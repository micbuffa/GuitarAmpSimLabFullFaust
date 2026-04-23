#!/usr/bin/env python3
"""
Compute per-model makeup gains for each tonestack preset.

Replicates the exact bilinear-transform IIR from IFCtonestackMulti.dsp
(D.T. Yeh tonestack model) and measures the RMS gain of a 440 Hz sine
at the default knob settings (bass=0.5, middle=0.3, treble=0.75).

Output: a lookup table ready to paste into the Faust DSP file.
"""

import numpy as np
from scipy import signal

SR = 48000  # sample rate

# ---- Component helpers (same as DSP) ----
def k(x): return x * 1e3
def M(x): return x * 1e6
def nF(x): return x * 1e-9
def pF(x): return x * 1e-12


def tonestack_coeffs(C1, C2, C3, R1, R2, R3, R4, t, m_knob, L, sr=SR):
    """
    Compute the IIR filter coefficients for the tonestack.
    Exact replica of the Faust tonestack() function.
    Returns (b_coeffs, a_coeffs) for scipy.signal.lfilter.
    """
    l = np.exp((L - 1) * 3.4)

    b1 = t*C1*R1 + m_knob*C3*R3 + l*(C1*R2 + C2*R2) + (C1*R3 + C2*R3)
    b2 = (t*(C1*C2*R1*R4 + C1*C3*R1*R4) - m_knob*m_knob*(C1*C3*R3*R3 + C2*C3*R3*R3)
          + m_knob*(C1*C3*R1*R3 + C1*C3*R3*R3 + C2*C3*R3*R3)
          + l*(C1*C2*R1*R2 + C1*C2*R2*R4 + C1*C3*R2*R4)
          + l*m_knob*(C1*C3*R2*R3 + C2*C3*R2*R3)
          + (C1*C2*R1*R3 + C1*C2*R3*R4 + C1*C3*R3*R4))
    b3 = (l*m_knob*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
          - m_knob*m_knob*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
          + m_knob*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
          + t*C1*C2*C3*R1*R3*R4 - t*m_knob*C1*C2*C3*R1*R3*R4
          + t*l*C1*C2*C3*R1*R2*R4)

    a0 = 1.0
    a1 = ((C1*R1 + C1*R3 + C2*R3 + C2*R4 + C3*R4)
          + m_knob*C3*R3 + l*(C1*R2 + C2*R2))
    a2 = (m_knob*(C1*C3*R1*R3 - C2*C3*R3*R4 + C1*C3*R3*R3 + C2*C3*R3*R3)
          + l*m_knob*(C1*C3*R2*R3 + C2*C3*R2*R3)
          - m_knob*m_knob*(C1*C3*R3*R3 + C2*C3*R3*R3)
          + l*(C1*C2*R2*R4 + C1*C2*R1*R2 + C1*C3*R2*R4 + C2*C3*R2*R4)
          + (C1*C2*R1*R4 + C1*C3*R1*R4 + C1*C2*R3*R4 + C1*C2*R1*R3
             + C1*C3*R3*R4 + C2*C3*R3*R4))
    a3 = (l*m_knob*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
          - m_knob*m_knob*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
          + m_knob*(C1*C2*C3*R3*R3*R4 + C1*C2*C3*R1*R3*R3 - C1*C2*C3*R1*R3*R4)
          + l*C1*C2*C3*R1*R2*R4
          + C1*C2*C3*R1*R3*R4)

    c = 2.0 * float(sr)
    B0 = -b1*c - b2*c**2 - b3*c**3
    B1 = -b1*c + b2*c**2 + 3*b3*c**3
    B2 =  b1*c + b2*c**2 - 3*b3*c**3
    B3 =  b1*c - b2*c**2 + b3*c**3
    A0 = -a0 - a1*c - a2*c**2 - a3*c**3
    A1 = -3*a0 - a1*c + a2*c**2 + 3*a3*c**3
    A2 = -3*a0 + a1*c + a2*c**2 - 3*a3*c**3
    A3 = -a0 + a1*c - a2*c**2 + a3*c**3

    # Faust: 1/A0 * fi.iir((B0,B1,B2,B3),(A1/A0,A2/A0,A3/A0))
    # scipy.signal.lfilter expects b = [B0/A0, B1/A0, B2/A0, B3/A0], a = [1, A1/A0, A2/A0, A3/A0]
    b_filt = np.array([B0, B1, B2, B3]) / A0
    a_filt = np.array([A0, A1, A2, A3]) / A0

    return b_filt, a_filt


# ---- Tonestack models (same component values as DSP) ----
models = {
    "Mesa Boogie": {
        "C1": pF(250), "C2": nF(100), "C3": nF(47),
        "R1": k(250),  "R2": k(250),  "R3": k(25), "R4": k(100),
    },
    "JCM800": {
        "C1": pF(470), "C2": nF(22), "C3": nF(22),
        "R1": k(220),  "R2": M(1),   "R3": k(22), "R4": k(33),
    },
    "AC30": {
        "C1": pF(50),  "C2": nF(22), "C3": nF(22),
        "R1": M(1),    "R2": M(1),   "R3": k(10), "R4": k(100),
    },
    "Fender Hot Rod": {
        "C1": pF(250), "C2": nF(100), "C3": nF(22),
        "R1": k(250),  "R2": k(250),  "R3": k(25), "R4": k(130),
    },
}

# ---- Default knob settings ----
TREBLE = 0.75
MIDDLE = 0.30
BASS   = 0.50

# ---- Measure RMS gain ----
import scipy.io.wavfile as wavfile
import os

def measure_rms_gain(b_filt, a_filt, freq=440, duration=0.5, sr=SR):
    """Feed signal.wav through the filter and measure output RMS / input RMS."""
    wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/signal.wav'
    if not os.path.exists(wav_path):
        wav_path = '../../../SwankyAmpFaust/SwankyAmpFaust/data/lo-gain.wav'
    
    sr_wav, data = wavfile.read(wav_path)
    if len(data.shape) > 1:
        data = data[:, 0]
    if data.dtype == np.int16:
        x = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        x = data.astype(np.float32) / 2147483648.0
    else:
        x = data.astype(np.float32)
        
    y = signal.lfilter(b_filt, a_filt, x)
    skip = int(sr_wav * 0.1)
    rms_in  = np.sqrt(np.mean(x[skip:]**2))
    rms_out = np.sqrt(np.mean(y[skip:]**2))
    return rms_out / rms_in

def measure_peak_freq_response(b_filt, a_filt, sr=SR):
    """Compute peak of the magnitude response across all frequencies."""
    w, h = signal.freqz(b_filt, a_filt, worN=8192, fs=sr)
    return np.max(np.abs(h))


if __name__ == "__main__":
    print("=" * 70)
    print("Tonestack Makeup Gain Computation")
    print(f"Settings: Bass={BASS}, Middle={MIDDLE}, Treble={TREBLE}, SR={SR}")
    print("=" * 70)
    print()

    gains_rms = []
    gains_peak = []

    for name, comp in models.items():
        b, a = tonestack_coeffs(
            comp["C1"], comp["C2"], comp["C3"],
            comp["R1"], comp["R2"], comp["R3"], comp["R4"],
            TREBLE, MIDDLE, BASS, SR
        )

        rms_gain = measure_rms_gain(b, a, freq=440)
        peak_gain = measure_peak_freq_response(b, a)
        rms_440 = measure_rms_gain(b, a, freq=440)
        rms_1k  = measure_rms_gain(b, a, freq=1000)
        rms_330 = measure_rms_gain(b, a, freq=330)

        # Makeup = inverse of RMS gain at 440 Hz (guitar fundamental)
        makeup_rms = 1.0 / rms_gain
        makeup_peak = 1.0 / peak_gain

        gains_rms.append(makeup_rms)
        gains_peak.append(makeup_peak)

        print(f"  {name:20s}:")
        print(f"    RMS gain @440Hz  = {rms_gain:.6f}  (makeup = {makeup_rms:.2f} = {20*np.log10(makeup_rms):+.1f} dB)")
        print(f"    RMS gain @330Hz  = {rms_330:.6f}")
        print(f"    RMS gain @1kHz   = {rms_1k:.6f}")
        print(f"    Peak freq resp   = {peak_gain:.6f}  (makeup = {makeup_peak:.2f} = {20*np.log10(makeup_peak):+.1f} dB)")
        print()

    # ---- Also sweep all knob positions to find worst-case ----
    print("-" * 70)
    print("Sweep Analysis: Min/Max RMS gain across all knob positions per model")
    print("-" * 70)
    print()
    
    for name, comp in models.items():
        min_rms = float('inf')
        max_rms = 0
        for bass_v in np.linspace(0, 1, 11):
            for mid_v in np.linspace(0, 1, 11):
                for treb_v in np.linspace(0, 1, 11):
                    b, a = tonestack_coeffs(
                        comp["C1"], comp["C2"], comp["C3"],
                        comp["R1"], comp["R2"], comp["R3"], comp["R4"],
                        treb_v, mid_v, bass_v, SR
                    )
                    rms = measure_rms_gain(b, a, freq=440)
                    min_rms = min(min_rms, rms)
                    max_rms = max(max_rms, rms)
        print(f"  {name:20s}: min={min_rms:.6f} ({20*np.log10(min_rms):+.1f}dB)  max={max_rms:.6f} ({20*np.log10(max_rms):+.1f}dB)  range={20*np.log10(max_rms/min_rms):.1f}dB")

    print()
    print("=" * 70)
    print("FAUST LOOKUP TABLE (paste into IFCtonestackMulti.dsp)")
    print("=" * 70)
    table_str = ", ".join(f"{g:.2f}" for g in gains_rms)
    print(f"makeupGainTable = {table_str};")
    print()
    print("// Current values in DSP: 10.0, 8.0, 12.5, 6.3")
    print(f"// New computed values:   {table_str}")
