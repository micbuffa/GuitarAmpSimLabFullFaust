import numpy as np
import scipy.io.wavfile as wavfile
from scipy import signal
import re
import os

# 1. Parse the IFCcabSim.dsp file to extract the FIR coefficients
with open('IFCcabSim.dsp', 'r') as f:
    content = f.read()

# Regular expression to match fir_X = fi.fir((...));
# The coefficients can span multiple lines, but in this file they are on one line.
matches = re.finditer(r'fir_(\d+)\s*=\s*fi\.fir\(\(([^)]+)\)\);', content)

firs = {}
for m in matches:
    idx = int(m.group(1))
    coefs_str = m.group(2)
    # Extract all floating point numbers
    coefs = [float(x) for x in coefs_str.split(',')]
    firs[idx] = np.array(coefs)

print(f"Loaded {len(firs)} FIRs: {list(firs.keys())}")

def simulate_cabsim(input_wav):
    sr, data = wavfile.read(input_wav)
    if len(data.shape) > 1:
        data = data[:, 0] # mono
    
    if data.dtype == np.int16:
        data = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        data = data.astype(np.float32) / 2147483648.0
        
    input_rms = np.sqrt(np.mean(data**2))
    print(f"Input RMS: {input_rms:.6f}")
    
    results = {}
    for idx, h in firs.items():
        # filter(b, a, x): b=h, a=[1.0]
        y = signal.lfilter(h, [1.0], data)
        # Skip transients
        skip = int(0.1 * sr)
        output_rms = np.sqrt(np.mean(y[skip:]**2))
        
        makeup_gain = input_rms / output_rms if output_rms > 0 else 1.0
        results[idx] = {
            'out_rms': output_rms,
            'makeup_gain': makeup_gain
        }
        
    return results

if __name__ == '__main__':
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    if not os.path.exists(wav_path):
        wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/lo-gain.wav'
        
    print(f"Using {wav_path} for simulation...")
    res = simulate_cabsim(wav_path)
    
    print("\nIR  | Output RMS | Makeup Gain")
    print("-" * 35)
    gains = []
    for idx in sorted(res.keys()):
        r = res[idx]['out_rms']
        g = res[idx]['makeup_gain']
        gains.append(g)
        print(f"{idx:3} | {r:10.6f} | {g:10.6f}")
        
    print("\nNew Faust Code:")
    print(f"cab_makeup_gain = {gains[0]:.6f}, {gains[1]:.6f}, {gains[2]:.6f} : ba.selectn(3, selected_cab);")
    
