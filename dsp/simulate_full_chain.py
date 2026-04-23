import numpy as np
import scipy.io.wavfile as wavfile
from scipy import signal
import os

SR = 48000

def smooth_tanh(x):
    x2 = x * x
    return x * (27.0 + x2) / (27.0 + 9.0 * x2)

def FtubeCurve_mock(x):
    # Mock Guitarix tube curve returning approximately x for small inputs
    return x

def poweramp_simulate(x_in, master_vols, pdrive=4.0, plevel_db=-3.0):
    env = np.zeros_like(x_in)
    s = 0.0
    for i in range(len(x_in)):
        s = s * 0.96 + np.abs(x_in[i]) * 0.04
        env[i] = s

    results = []
    
    # dcblocker
    p_dc = np.exp(-2 * np.pi * 35.0 / SR)
    b_dc = [ (1+p_dc)/2, -(1+p_dc)/2 ]
    a_dc = [ 1, -p_dc ]
    
    for mv in master_vols:
        x_mv = x_in * mv
        
        driven = np.clip(x_mv * pdrive + env, -3.0, 3.0)
        m_coef = (smooth_tanh(driven) / smooth_tanh(1.0)) * 0.24
        
        y_out = np.zeros_like(x_in)
        ap_x1, ap_y1, dc_x1, dc_y1 = 0.0, 0.0, 0.0, 0.0
        
        for i in range(len(x_in)):
            m = m_coef[i]
            ap_out = m * x_mv[i] + ap_x1 - m * ap_y1
            ap_x1 = x_mv[i]
            ap_y1 = ap_out
            
            dc_out = b_dc[0] * ap_out + b_dc[1] * dc_x1 - a_dc[1] * dc_y1
            dc_x1 = ap_out
            dc_y1 = dc_out
            
            y_out[i] = dc_out
            
        plevel_lin = 10**(plevel_db/20.0)
        y_final = y_out * 4.0 * plevel_lin
        
        skip = int(SR * 0.1)
        rms_in  = np.sqrt(np.mean(x_in[skip:]**2)) # bypassed 
        rms_out = np.sqrt(np.mean(y_final[skip:]**2))
        results.append((rms_in, rms_out, np.max(np.abs(y_final[skip:]))))
        
    return results

def peaking_eq(fc, gain_db, Q, sr):
    A  = 10**(gain_db/40.0)
    w0 = 2*np.pi*fc/sr
    alpha = np.sin(w0)/(2*Q)
    b0 =  1 + alpha*A
    b1 = -2*np.cos(w0)
    b2 =  1 - alpha*A
    a0 =  1 + alpha/A
    a1 = -2*np.cos(w0)
    a2 =  1 - alpha/A
    return np.array([b0, b1, b2])/a0, np.array([a0, a1, a2])/a0

def shelving(fc, gain_db, shelf_type='low', sr=48000):
    # simple approximation for simulation
    A  = 10**(gain_db/40.0)
    w0 = 2*np.pi*fc/sr
    alpha = np.sin(w0)/2 * np.sqrt( (A + 1/A)*(1/1.0 - 1) + 2 ) # Q=1
    
    out_b, out_a = [1], [1]
    return out_b, out_a # Not perfectly simulating shelving, but peaking is enough

if __name__ == '__main__':
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    if not os.path.exists(wav_path):
        wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/lo-gain.wav'
        
    sr, data = wavfile.read(wav_path)
    if len(data.shape) > 1: data = data[:, 0]
    if data.dtype == np.int16: x_raw = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32: x_raw = data.astype(np.float32) / 2147483648.0
    else: x_raw = data.astype(np.float32)
    
    # 1. Preamp Guitarix Output (typically 0.013 RMS for signal.wav)
    # Using previous baseline
    x_preamp = x_raw * 0.25 # arbitrary scaling corresponding to typical hot output
    preamp_rms = np.sqrt(np.mean(x_preamp**2))
    print(f"Preamp RMS output: {preamp_rms:.6f}")
    
    # 2. EQ Block
    b_eq1, a_eq1 = peaking_eq(1222.4, 2.4, 0.26, SR)
    b_eq2, a_eq2 = peaking_eq(3943.9, 13.63, 2.35, SR)
    
    x_eq = signal.lfilter(b_eq1, a_eq1, x_preamp)
    x_eq = signal.lfilter(b_eq2, a_eq2, x_eq)
    # add simple low shelf boost approximation
    x_eq = x_eq * 10**(8.52/20.0) 
    
    eq_rms = np.sqrt(np.mean(x_eq**2))
    print(f"Post-EQ RMS output: {eq_rms:.6f}")
    
    # 3. Tonestack Makeup
    # The Tonestack reduces gain by ~15dB, then we multiply by 6.93 (+16dB)
    # Assuming net gain is roughly 1.0 for the raw guitar frequency curve
    x_ts = x_eq * 1.5 # Overall tonestack might still pass higher RMS due to EQ alignment
    
    ts_rms = np.sqrt(np.mean(x_ts**2))
    print(f"Tonestack RMS output (PowerAmp Input): {ts_rms:.6f}")
    
    # 4. PowerAmp
    mvs = [0.1, 1.0, 2.3, 4.0]
    res = poweramp_simulate(x_ts, mvs)
    
    print("\nMV    | Bypassed RMS (In) | Engaged RMS | Engaged Peak")
    print("-" * 55)
    for mv, (r_in, r_out, p_out) in zip(mvs, res):
        print(f"{mv:.2f} | {r_in:17.6f} | {r_out:11.6f} | {p_out:12.6f}")
