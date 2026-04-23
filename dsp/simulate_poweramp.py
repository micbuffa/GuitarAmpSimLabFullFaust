import numpy as np
import scipy.io.wavfile as wavfile
from scipy import signal
import os

SR = 48000

def smooth_tanh(x):
    x2 = x * x
    return x * (27.0 + x2) / (27.0 + 9.0 * x2)

def transfer(x, pcurve=1.0):
    return smooth_tanh(pcurve * x) / smooth_tanh(pcurve)

def envelope_follower(x, alpha=0.04):
    y = np.zeros_like(x)
    s = 0.0
    for i in range(len(x)):
        s = s * (1.0 - alpha) + np.abs(x[i]) * alpha
        y[i] = s
    return y

def get_peaking_filter(fc, gain_db, Q, sr=48000):
    # webaudio.lib peaking2 roughly translates to a peaking EQ
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

def simulate_poweramp_full(input_wav, master_vols):
    sr, data = wavfile.read(input_wav)
    if len(data.shape) > 1:
        data = data[:, 0]
    
    if data.dtype == np.int16:
        x_raw = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        x_raw = data.astype(np.float32) / 2147483648.0
    else:
        x_raw = data.astype(np.float32)
        
    # Assume the signal coming into PowerAmp is roughly 0.1 RMS typical of guitar levels
    x = x_raw * (0.1 / np.sqrt(np.mean(x_raw**2)))
    
    # Presence filter (two peaking filters at 2000 and 4000 Hz, gain=0 default)
    # Since gain=0, presence filter is flat! (b=[1], a=[1])
    # gainNFL = -0.4
    gainNFL = -0.4
    
    # dcblocker = fi.dcblocker (1 pole highpass around 35Hz)
    p_dc = np.exp(-2 * np.pi * 35.0 / sr)
    b_dc = [ (1+p_dc)/2, -(1+p_dc)/2 ]
    a_dc = [ 1, -p_dc ]

    results = []
    
    for mv in master_vols:
        # main = *(masterVolume) :(+ : waveshaper : fi.dcblocker) ~ feedbackCircuit : gain
        # x_in = input * mv
        x_in = x * mv
        
        # drive envelope mapped to allpass coefficient
        env = envelope_follower(x_in)
        driven = np.clip(x_in * 4.0 + env, -3.0, 3.0)
        m_coef = transfer(driven, 1.0) * 0.24
        
        N = len(x_in)
        y_waveshaper = np.zeros(N)
        y_feedback = np.zeros(N)
        y_out = np.zeros(N)
        
        # State variables
        dc_x1 = 0.0
        dc_y1 = 0.0
        ap_x1 = 0.0
        ap_y1 = 0.0
        
        for i in range(N):
            # Sum logic limit: (input + feedback)
            sum_in = x_in[i] + y_feedback[i]
            
            # Waveshaper (allpass)
            m = m_coef[i] # Modulated coefficient
            ap_out = m * sum_in + ap_x1 - m * ap_y1
            ap_x1 = sum_in
            ap_y1 = ap_out
            
            # DC Blocker
            dc_out = b_dc[0] * ap_out + b_dc[1] * dc_x1 - a_dc[1] * dc_y1
            dc_x1 = ap_out
            dc_y1 = dc_out
            
            # Feedback circuit (Presence is flat, so just gainNFL)
            feed_next = dc_out * gainNFL
            if i+1 < N:
                y_feedback[i+1] = feed_next
                
            # Output of ~ loop is dc_out
            y_out[i] = dc_out * 4.0 # plevel=1.0 default, gain=4.0
            
        skip = int(sr * 0.1)
        rms_in  = np.sqrt(np.mean(x_in[skip:]**2)) # Bypassed RMS (input to module)
        rms_out = np.sqrt(np.mean(y_out[skip:]**2)) # Engaged RMS
        
        results.append((rms_in, rms_out))
        
    return results

if __name__ == '__main__':
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    if not os.path.exists(wav_path):
        wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/lo-gain.wav'
        
    mvs = np.linspace(0.1, 4.0, 40)
    ref_mv = 2.3
    mvs[np.argmin(np.abs(mvs - ref_mv))] = ref_mv
    
    levels = simulate_poweramp_full(wav_path, mvs)
    
    print("Master Vol | Bypassed (In*MV) | Engaged RMS | Insertion Gain")
    print("-" * 65)
    for mv, (rms_in, rms_out) in zip(mvs, levels):
        insertion = rms_out / rms_in if rms_in > 0 else 1.0
        if int(mv*10)%5 == 0 or mv == ref_mv:
            print(f"{mv:10.2f} | {rms_in:16.6f} | {rms_out:11.6f} | {insertion:12.4f}")
            
    # Print the specific condition of Bypassed vs Engaged at MV=2.3
    idx_ref = np.where(mvs == ref_mv)[0][0]
    rms_in_ref, rms_out_ref = levels[idx_ref]
    
    print("\n--- Bypass Analysis at MV=2.3 ---")
    print(f"Input RMS (what bypass sees): 0.100000") # bypass doesn't have MV, it bypasses the whole module!
    print(f"Engaged RMS (output of module): {rms_out_ref:.6f}")
    bypass_ratio = 0.1 / rms_out_ref
    print(f"Bypass is {bypass_ratio:.2f}x louder/quieter than Engaged.")
