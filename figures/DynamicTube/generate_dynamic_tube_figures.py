"""
Dynamic Tube Model — Scientific Figures
Author: Michel Buffa / Antigravity
"""

import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
SR = 44100

plt.rcParams.update({
    'font.family': 'serif',
    'font.size': 11,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'legend.fontsize': 10,
    'figure.dpi': 150,
    'lines.linewidth': 1.6,
    'axes.grid': True,
    'grid.alpha': 0.3,
})
COLORS = ['#1f77b4', '#d62728', '#2ca02c', '#ff7f0e', '#9467bd']

# ─── DSP primitives ──────────────────────────────────────────────────────────

def env_follower(x, tau_ms):
    alpha = 1.0 / max(1.0, (tau_ms * SR * 0.001))
    y = np.zeros_like(x)
    abs_x = np.abs(x)
    y[0] = abs_x[0] * alpha
    for n in range(1, len(x)):
        y[n] = y[n-1] + alpha * (abs_x[n] - y[n-1])
    return y

def dynamic_tube(x, drive0, bias0, kF, kS, kB, tauF, tauS):
    eF = env_follower(x, tauF)
    eS = env_follower(x, tauS)
    
    drive_dyn = np.clip(drive0 + kF * eF - kS * eS, 0.05, 40.0)
    bias_dyn = np.clip(bias0 + kB * eS, -1.5, 1.5)
    
    y = np.tanh(drive_dyn * (x + bias_dyn))
    return y, eF, eS, drive_dyn, bias_dyn


# ─── Figure 1: Envelopes and Modulation ──────────────────────────────────────
def fig1_envelopes_and_mods():
    N = int(SR * 0.6)
    t_ms = np.arange(N) / SR * 1000
    f = 100.0
    # Burst signal: loud then soft
    x = np.where(t_ms < 300, 0.8, 0.2) * np.sin(2*np.pi * f * np.arange(N)/SR)
    
    # 12AX7-like params
    drive0, bias0 = 4.5, -0.15
    kF, kS, kB = 4.0, 1.2, 0.15
    tauF, tauS = 5.0, 120.0
    
    y, eF, eS, dDyn, bDyn = dynamic_tube(x, drive0, bias0, kF, kS, kB, tauF, tauS)

    fig, axes = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

    # (a) Input & Envelopes
    ax = axes[0]
    ax.plot(t_ms, x, color='gray', alpha=0.3, lw=0.8, label='Input signal $x(t)$')
    ax.plot(t_ms, eF, color=COLORS[0], label=f'Fast Envelope $e_F$ (τ={tauF}ms)')
    ax.plot(t_ms, eS, color=COLORS[1], lw=2, label=f'Slow Envelope $e_S$ (τ={tauS}ms)')
    ax.set_ylabel('Amplitude')
    ax.set_title('(a) Input signal and envelope tracking')
    ax.legend(loc='upper right', fontsize=9)

    # (b) Drive Modulation
    ax = axes[1]
    ax.axhline(drive0, color='black', ls='--', lw=1, label='Static drive $d_0$')
    ax.plot(t_ms, dDyn, color=COLORS[2], label='Dynamic Drive $d(t) = d_0 + k_F e_F - k_S e_S$')
    ax.set_ylabel('Drive Gain')
    ax.set_title(f'(b) Drive Modulation (Touch Sensitivity $k_F={kF}$, Sag $k_S={kS}$)')
    ax.legend(loc='upper right', fontsize=9)

    # (c) Bias Modulation
    ax = axes[2]
    ax.axhline(bias0, color='black', ls='--', lw=1, label='Static bias $b_0$')
    ax.plot(t_ms, bDyn, color=COLORS[3], label='Dynamic Bias $b(t) = b_0 + k_B e_S$')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Bias Offset')
    ax.set_title(f'(c) Bias Shift (Operating Point Modulation $k_B={kB}$)')
    ax.legend(loc='upper right', fontsize=9)

    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_dyn1_modulations.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: Static vs Dynamic Comparison ─────────────────────────────────
def fig2_comparison():
    N = int(SR * 0.4)
    t_ms = np.arange(N) / SR * 1000
    f = 150.0
    x = 0.85 * np.sin(2*np.pi * f * np.arange(N)/SR)
    
    # High kS to emphasize sag
    y_dyn, _, _, dDyn, _ = dynamic_tube(x, 6.0, -0.15, 0.0, 3.5, 0.0, 5.0, 60.0)
    y_stat = np.tanh(6.0 * (x - 0.15))

    fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))

    # (a) Time domain comparison (Envelope sag)
    ax = axes[0]
    # Calculate rolling RMS for visual sag
    w = int(0.005 * SR)
    rms_stat = [np.sqrt(np.mean(y_stat[max(0,i-w):i+w]**2)) for i in range(0, N, w//2)]
    rms_dyn = [np.sqrt(np.mean(y_dyn[max(0,i-w):i+w]**2)) for i in range(0, N, w//2)]
    t_rms = np.arange(len(rms_stat)) * (w//2) / SR * 1000
    
    ax.plot(t_rms, rms_stat, 'k--', label='Static Model (Fixed Gain)')
    ax.plot(t_rms, rms_dyn, color=COLORS[1], lw=2, label='Dynamic Model (with Sag)')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output RMS')
    ax.set_title('(a) Power Supply Sag Effect\nHigh gain followed by compression')
    ax.legend(fontsize=9)

    # (b) Distorted waveforms (Asymmetry shift)
    ax = axes[1]
    # Trigger bias shift
    y_dyn_b, _, _, _, bDyn = dynamic_tube(x, 5.0, -0.1, 0.0, 0.0, 0.4, 5.0, 30.0)
    # Compare start vs end
    seg1 = slice(int(0.02*SR), int(0.04*SR)) # start
    seg2 = slice(int(0.35*SR), int(0.37*SR)) # end (after shift)
    
    ax.plot(np.arange(len(x[seg1])), y_dyn_b[seg1], color=COLORS[0], alpha=0.5, label='Start of note')
    ax.plot(np.arange(len(x[seg2])), y_dyn_b[seg2], color=COLORS[1], label='After bias shift')
    ax.set_title('(b) Dynamic Asymmetry Shift\nOperating point crawls over time')
    ax.set_ylabel('Amplitude')
    ax.legend(fontsize=9)

    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_dyn2_comparison.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 3: Harmonic Evolution (Spectrogram/Waterfall-style) ──────────────
def fig3_harmonics():
    N = int(SR * 1.0)
    f0 = 150.0
    # Sustained note
    x = 0.8 * np.sin(2*np.pi * f0 * np.arange(N)/SR)
    
    # Focus on bias shift kB=0.6, tauS=200ms
    y, _, _, _, b_trace = dynamic_tube(x, 5.0, 0.0, 0.0, 0.0, 0.6, 50.0, 200.0)
    
    # Harmonic amplitudes over time
    win_size = int(0.05 * SR)
    hop = win_size // 2
    times = []
    h2_vals = []
    h3_vals = []
    
    for i in range(0, N - win_size, hop):
        seg = y[i:i+win_size] * np.hanning(win_size)
        spec = np.abs(np.fft.rfft(seg)) / (win_size/2)
        freqs = np.fft.rfftfreq(win_size, 1/SR)
        
        # Extract H2 (300Hz) and H3 (450Hz)
        h2_idx = np.argmin(np.abs(freqs - 2*f0))
        h3_idx = np.argmin(np.abs(freqs - 3*f0))
        
        times.append(i / SR * 1000)
        h2_vals.append(spec[h2_idx])
        h3_vals.append(spec[h3_idx])

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(times, h2_vals, color=COLORS[1], lw=2.5, label='Even Harmonic (H2)')
    ax.plot(times, h3_vals, color=COLORS[0], lw=1.5, ls='--', label='Odd Harmonic (H3)')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Harmonic Amplitude')
    ax.set_title('Dynamic Harmonic Balance (Dynamic Bias Shift)\nH2 grows while H3 remains stable')
    ax.fill_between(times, h2_vals, color=COLORS[1], alpha=0.1)
    ax.legend()
    
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_dyn3_harmonics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


if __name__ == '__main__':
    print("Generating dynamic tube model figures...\n")
    fig1_envelopes_and_mods()
    fig2_comparison()
    fig3_harmonics()
    print("\nAll figures saved in:", OUT_DIR)
