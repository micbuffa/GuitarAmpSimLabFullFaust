import numpy as np
import scipy.io.wavfile as wavfile
from scipy import signal
import re

with open('IFCguitarixTubes.dsp', 'r') as f:
    content = f.read()

tables = {}
matches = re.finditer(r'wf_([a-zA-Z0-9_]+)\s*=\s*waveform{([^}]+)};', content)
for m in matches:
    name = m.group(1)
    vals = [float(x) for x in m.group(2).split(',')]
    tables[name] = np.array(vals)

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
    Rp = 100.0e3
    VkC = Vk0 * (Rp/Rk)
    feed_gain = Rk/Rp
    p_lpf = np.exp(-2.0 * np.pi * fck / sr)
    b0_lpf = 1.0 - p_lpf
    a1_lpf = p_lpf
    p_hpf = np.exp(-2.0 * np.pi * 31.0 / sr)
    b0_hpf = (1.0 + p_hpf) / 2.0
    N = len(x)
    tube_out = np.zeros(N)
    hpf_out = np.zeros(N)
    lpf_s = 0.0
    x_prev = 0.0
    y_prev = 0.0
    for i in range(N):
        vin = x[i] + lpf_s
        v_diff = vin - Vk0
        f_val = FtubeCurve(wf_array, v_diff)
        t_out = f_val + (VkC - vplus)
        fb_in = t_out * feed_gain
        lpf_s = fb_in * b0_lpf + lpf_s * a1_lpf
        y_tube = t_out / divider
        y_h = b0_hpf * (y_tube - x_prev) + p_hpf * y_prev
        hpf_out[i] = y_h
        x_prev = y_tube
        y_prev = y_h
    return hpf_out

def process_lowpass(x, fc, sr):
    p = np.exp(-2.0 * np.pi * fc / sr)
    b0 = 1.0 - p
    a1 = p
    return signal.lfilter([b0], [1.0, -a1], x)

def process_highpass(x, fc, sr):
    p = np.exp(-2.0 * np.pi * fc / sr)
    b0 = (1.0 + p) / 2.0
    return signal.lfilter([b0, -b0], [1.0, -p], x)

def simulate_preamp_stages(input_wav):
    sr, data = wavfile.read(input_wav)
    if len(data.shape) > 1:
        data = data[:, 0]
    if data.dtype == np.int16:
        data = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        data = data.astype(np.float32) / 2147483648.0
        
    input_vol = 10**(-6.0/20) # 0.501
    preamp_gk = 10**(-6.0/20) # 0.501
    
    s_in = data * input_vol
    
    wf_68k = tables['12AX7_68k']
    wf_250k = tables['12AX7_250k']
    
    # Stage 1
    y1 = process_tubestageF(s_in, wf_68k, 250.0, 40.0, 86.0, 2700.0, 1.581656, sr)
    
    # Stage 2
    y_in2 = process_lowpass(y1, 6531.0, sr) * preamp_gk
    y2 = process_tubestageF(y_in2, wf_250k, 250.0, 40.0, 132.0, 1500.0, 1.204285, sr)
    
    # Stage 3
    y_in3 = process_lowpass(y2, 6531.0, sr) * preamp_gk
    y3 = process_tubestageF(y_in3, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
    
    # Stage 4
    y_in4 = process_lowpass(y3, 6531.0, sr)
    y_in4 = process_highpass(y_in4, 100.0, sr) * preamp_gk
    y4 = process_tubestageF(y_in4, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
    
    # Stage 5
    y_in5 = process_lowpass(y4, 6531.0, sr)
    y_in5 = process_highpass(y_in5, 100.0, sr) * preamp_gk
    y5 = process_tubestageF(y_in5, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
    
    skip = int(0.5 * sr)
    rms_vals = [
        np.sqrt(np.mean(y1[skip:]**2)),
        np.sqrt(np.mean(y2[skip:]**2)),
        np.sqrt(np.mean(y3[skip:]**2)),
        np.sqrt(np.mean(y4[skip:]**2)),
        np.sqrt(np.mean(y5[skip:]**2))
    ]
    peak_vals = [
        np.max(np.abs(y1[skip:])),
        np.max(np.abs(y2[skip:])),
        np.max(np.abs(y3[skip:])),
        np.max(np.abs(y4[skip:])),
        np.max(np.abs(y5[skip:]))
    ]
    
    return rms_vals, peak_vals
    
def simulate_sweep():
    sr = 48000
    t = np.linspace(0, 1.0, sr)
    data = np.sin(2 * np.pi * 400 * t)
    
    input_vol = 10**(-6.0/20)
    s_in = data * input_vol
    
    wf_68k = tables['12AX7_68k']
    wf_250k = tables['12AX7_250k']
    
    # Base stage 1
    y1 = process_tubestageF(s_in, wf_68k, 250.0, 40.0, 86.0, 2700.0, 1.581656, sr)
    skip = int(0.5 * sr)
    rms1 = np.sqrt(np.mean(y1[skip:]**2))
    
    preamp_dbs = np.linspace(-20, 20, 9)
    
    results = {1: [], 2: [], 3: [], 4: [], 5: []}
    
    for db in preamp_dbs:
        preamp_gk = 10**(db / 20)
        
        y_in2 = process_lowpass(y1, 6531.0, sr) * preamp_gk
        y2 = process_tubestageF(y_in2, wf_250k, 250.0, 40.0, 132.0, 1500.0, 1.204285, sr)
        
        y_in3 = process_lowpass(y2, 6531.0, sr) * preamp_gk
        y3 = process_tubestageF(y_in3, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
        
        y_in4 = process_highpass(process_lowpass(y3, 6531.0, sr), 100.0, sr) * preamp_gk
        y4 = process_tubestageF(y_in4, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
        
        y_in5 = process_highpass(process_lowpass(y4, 6531.0, sr), 100.0, sr) * preamp_gk
        y5 = process_tubestageF(y_in5, wf_250k, 250.0, 40.0, 194.0, 820.0, 0.840703, sr)
        
        results[1].append(rms1) # Constant
        results[2].append(np.sqrt(np.mean(y2[skip:]**2)))
        results[3].append(np.sqrt(np.mean(y3[skip:]**2)))
        results[4].append(np.sqrt(np.mean(y4[skip:]**2)))
        results[5].append(np.sqrt(np.mean(y5[skip:]**2)))
        
    # Get reference values at -6dB
    idx_ref = np.where(np.isclose(preamp_dbs, -6.0))[0][0]
    
    print(f"Preamp gains (dB): {preamp_dbs}")
    for n in range(2, 6):
        ref = results[n][idx_ref]
        ratios = [ref / val for val in results[n]]
        print(f"Comp Ratios for {n} stages: {[round(r, 3) for r in ratios]}")

if __name__ == '__main__':
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    rms_vals, peak_vals = simulate_preamp_stages(wav_path)
    ref_rms = rms_vals[0]
    gains = [ref_rms / r for r in rms_vals]
    print("RMS Values (Loud Sine):", rms_vals)
    print("Peak Values (Loud Sine):", peak_vals)
    print("Stage Comp Gains (Linear):", gains)
    
    compensated_peaks = [p * g for p, g in zip(peak_vals, gains)]
    print("Compensated Peaks (Loud Sine):", compensated_peaks)
