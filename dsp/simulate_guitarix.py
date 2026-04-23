import numpy as np
import scipy.io.wavfile as wavfile
from scipy import signal
import re
import os

# 1. Parse the IFCguitarixTubes.dsp file to extract the wavetables
with open('IFCguitarixTubes.dsp', 'r') as f:
    content = f.read()

tables = {}
matches = re.finditer(r'wf_([a-zA-Z0-9_]+)\s*=\s*waveform{([^}]+)};', content)
for m in matches:
    name = m.group(1)
    vals = [float(x) for x in m.group(2).split(',')]
    tables[name] = np.array(vals)

print("Loaded wavetables:", list(tables.keys()))

# 2. Tube Stage implementation
def FtubeCurve(wf_array, x_in):
    low = -5.0
    istep = 200.0
    tsize = 2001
    
    f = (x_in - low) * istep
    f = np.clip(f, 0.0, tsize - 1.0000001)
    i = np.floor(f).astype(int)
    f_rem = f - i
    val0 = wf_array[i]
    val1 = wf_array[i+1]
    return val0 * (1.0 - f_rem) + val1 * f_rem

def process_tubestageF(x, wf_array, vplus, divider, fck, Rk, Vk0, sr):
    """
    tubestageF(FtubeCurve,vplus,divider,fck,Rk,Vk0) = tube : hpf with {
        lpfk = fi.lowpass(1,fck);
        Rp = 100.0e3;
        VkC = Vk0 * (Rp/Rk);
        tube = (+ : -(Vk0) : FtubeCurve : +(VkC-vplus)) ~ (*(Rk/Rp) : lpfk) : /(divider);
        hpf = fi.highpass(1,31.0);
    };
    """
    Rp = 100.0e3
    VkC = Vk0 * (Rp/Rk)
    feed_gain = Rk/Rp
    
    # LPF pole
    p_lpf = np.exp(-2.0 * np.pi * fck / sr)
    b0_lpf = 1.0 - p_lpf
    a1_lpf = p_lpf
    
    # HPF pole
    p_hpf = np.exp(-2.0 * np.pi * 31.0 / sr)
    # Faust highpass 1-pole: 
    # y = (x - x')*0.5*(1+p) + p*y' -> b = [b0, -b0], a = [1, -p]
    # where b0 = (1 + p) / 2
    b0_hpf = (1.0 + p_hpf) / 2.0
    
    N = len(x)
    tube_out = np.zeros(N)
    hpf_out = np.zeros(N)
    
    # state
    lpf_s = 0.0
    x_prev = 0.0
    y_prev = 0.0
    
    for i in range(N):
        # 1. tube input sum
        vin = x[i] + lpf_s
        
        # 2. tube nonlinearity
        v_diff = vin - Vk0
        f_val = FtubeCurve(wf_array, v_diff)
        
        # 3. tube output
        t_out = f_val + (VkC - vplus)
        
        # 4. feedback path
        fb_in = t_out * feed_gain
        lpf_s = fb_in * b0_lpf + lpf_s * a1_lpf
        
        # 5. divider
        y_tube = t_out / divider
        tube_out[i] = y_tube
        
        # 6. hpf
        # hpf_s = b0_hpf * (y_tube - x_prev) + p_hpf * y_prev
        y_h = b0_hpf * (y_tube - x_prev) + p_hpf * y_prev
        hpf_out[i] = y_h
        
        x_prev = y_tube
        y_prev = y_h
        
    return hpf_out

# 3. Lowpass filter 1-pole FC=6531 Hz
def process_lowpass(x, fc, sr):
    p = np.exp(-2.0 * np.pi * fc / sr)
    b0 = 1.0 - p
    a1 = p
    b = [b0]
    a = [1.0, -a1]
    return signal.lfilter(b, a, x)

# 4. Measure overall output RMS curve
def simulate_preamp(input_wav, gain_dbs=[]):
    sr, data = wavfile.read(input_wav)
    if len(data.shape) > 1:
        data = data[:, 0] # mono
    
    if data.dtype == np.int16:
        data = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        data = data.astype(np.float32) / 2147483648.0
        
    input_vol = 10**(-6.0/20) # -6 dB input vol
    s1_in = data * input_vol
    
    # Precompute stage 1 (T1_12AX7)
    # T1_12AX7 = tubestage(Ftube_12AX7_68k,86.0,2700.0,1.581656)
    wf_68k = tables['12AX7_68k']
    s1_out = process_tubestageF(s1_in, wf_68k, vplus=250.0, divider=40.0, fck=86.0, Rk=2700.0, Vk0=1.581656, sr=sr)
    
    results = []
    
    # T2_12AX7 = tubestage(Ftube_12AX7_250k,132.0,1500.0,1.204285)
    wf_250k = tables['12AX7_250k']
    
    for gain_db in gain_dbs:
        gk = 10**(gain_db / 20)
        
        # Interstage 1-2
        y = process_lowpass(s1_out, 6531.0, sr)
        y = y * gk
        
        # Stage 2
        y = process_tubestageF(y, wf_250k, vplus=250.0, divider=40.0, fck=132.0, Rk=1500.0, Vk0=1.204285, sr=sr)
        
        # Compute RMS
        skip = int(0.5 * sr) # Give it 0.5s to settle transients
        rms = np.sqrt(np.mean(y[skip:]**2))
        results.append(rms)
        
    return results

if __name__ == '__main__':
    test_gains_db = np.linspace(-20, 20, 21)
    
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    print(f"Using {wav_path} for simulation...")
    rms_levels = simulate_preamp(wav_path, gain_dbs=test_gains_db)
    
    idx_ref = np.where(np.isclose(test_gains_db, -6.0))[0][0]
    ref_rms = rms_levels[idx_ref]
    
    print(f"Reference RMS @ -6 dB: {ref_rms:.6f}")
    print("\nGain (dB) | Output RMS | Comp Ratio")
    print("-" * 40)
    for g, r in zip(test_gains_db, rms_levels):
        comp = ref_rms / r if r > 0 else 1.0
        print(f"{g:7.1f} | {r:10.6f} | {comp:10.4f}")
        
    gain_linear = 10**(test_gains_db / 20)
    comp_factors = ref_rms / np.array(rms_levels)
    ref_linear = 10**(-6.0/20)
    
    from scipy.optimize import curve_fit
    def tanh_comp(g_lin, sens):
        return np.tanh(sens * ref_linear) / np.maximum(0.001, np.tanh(sens * g_lin))
        
    try:
        popt, _ = curve_fit(tanh_comp, gain_linear, comp_factors, p0=[5.0])
        best_sens = popt[0]
        print(f"\nOptimized tube_sens: {best_sens:.4f}")
        best_comp = tanh_comp(gain_linear, best_sens)
        print(f"Max error with optimized formula: {np.max(np.abs(best_comp - comp_factors)):.4f}")
        print(f"\nNew Faust Formula:")
        print(f"tube_sens = {best_sens:.4f};")
        print(f"gk_comp_ratio = ma.tanh(tube_sens * ref_linear) / max(0.001, ma.tanh(tube_sens * preamp));")
    except Exception as e:
        print("Optimization failed:", e)
