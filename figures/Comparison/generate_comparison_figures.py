"""
Model Comparison — Static (Coupled) vs Dynamic (Gray-box)
For: "Comparative Analysis of Phenomological and Gray-Box Tube Models"
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

def iir_lp(x, tau_ms):
    alpha = 1.0 / max(1.0, (tau_ms * SR * 0.001))
    y = np.zeros_like(x)
    y[0] = x[0] * alpha
    for n in range(1, len(x)):
        y[n] = y[n-1] + alpha * (x[n] - y[n-1])
    return y

def env_follower(x, tau_ms):
    return iir_lp(np.abs(x), tau_ms)

# --- Model 1: Phenomenologically Coupled ---
def model1_coupled(x, a, b, gc_tau, gc_ain, gc_abias, cb_tau, cb_scale):
    # Grid current
    vth = -0.1
    v_cl = np.maximum(vth, x)
    v_gc = iir_lp(v_cl, gc_tau)
    z = (x - gc_ain * v_gc) + gc_abias * v_gc
    
    # Cathode bias
    v_rect = np.maximum(0.0, z)
    cb = iir_lp(v_rect, cb_tau) * cb_scale
    
    # Nonlinearity
    y = np.tanh(a * z + b - cb)
    return y, cb, v_gc

# --- Model 2: Dynamic Gray-box ---
def model2_dynamic(x, drive0, bias0, kF, kS, kB, tauF, tauS):
    eF = env_follower(x, tauF)
    eS = env_follower(x, tauS)
    
    drive_dyn = np.clip(drive0 + kF * eF - kS * eS, 0.05, 40.0)
    bias_dyn = np.clip(bias0 + kB * eS, -1.5, 1.5)
    
    y = np.tanh(drive_dyn * (x + bias_dyn))
    return y, drive_dyn, bias_dyn


# ─── Figure 1: Step Response and Sag Comparison ──────────────────────────────
def fig1_step_sag():
    N = int(SR * 0.8)
    t_ms = np.arange(N) / SR * 1000
    f = 150.0
    # Sustained burst
    x = np.where(t_ms < 500, 0.85, 0.0) * np.sin(2*np.pi * f * np.arange(N)/SR)
    
    # Calibrated params
    # Model 1 (Marshall-ish)
    y1, cb, vgc = model1_coupled(x, 6.0, -0.12, 180, 0.8, 1.5, 68.0, 0.12)
    # Model 2 (Equivalent calibration)
    y2, dDyn, bDyn = model2_dynamic(x, 5.0, -0.15, 0.0, 1.8, 0.22, 5.0, 80.0)

    fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

    # (a) Output RMS Comparison
    ax = axes[0]
    w = int(0.005 * SR) # 5ms RMS
    rms1 = [np.sqrt(np.mean(y1[max(0,i-w):i+w]**2)) for i in range(0, N, w//2)]
    rms2 = [np.sqrt(np.mean(y2[max(0,i-w):i+w]**2)) for i in range(0, N, w//2)]
    t_rms = np.arange(len(rms1)) * (w//2) / SR * 1000
    
    ax.plot(t_rms, rms1, color=COLORS[0], lw=2.5, label='Model 1 (Coupled GC + CB)')
    ax.plot(t_rms, rms2, color=COLORS[1], lw=1.5, ls='--', label='Model 2 (Dynamic Gray-box)')
    ax.set_ylabel('Output RMS')
    ax.set_title('(a) Envelope Sag and Compression Response\nSustained burst at $f=150$Hz')
    ax.legend()

    # (b) Internal Tracking
    ax = axes[1]
    ax.plot(t_ms, cb, color=COLORS[0], alpha=0.7, label='Model 1: Cathode Bias $(\\Delta V_k)$')
    # Scale Model 2 bias change for visual comparison
    ax.plot(t_ms, np.abs(bDyn - (-0.15)), color=COLORS[1], ls='--', label='Model 2: Dynamic Bias shift $|b(t) - b_0|$')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Parameter Shift')
    ax.set_title('(b) Evolution of Dynamic Bias / Sag Parameters')
    ax.legend(fontsize=9)

    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_comp1_sag.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: Dynamic Harmonic Balance ──────────────────────────────────────
def fig2_harmonics():
    N = int(SR * 1.5)
    f0 = 200.0
    x = 0.75 * np.sin(2*np.pi * f0 * np.arange(N)/SR)
    
    y1, _, _ = model1_coupled(x, 6.0, -0.1, 180, 0.8, 1.5, 60.0, 0.20)
    y2, _, _ = model2_dynamic(x, 4.5, -0.1, 0.0, 1.2, 0.5, 5.0, 60.0)

    # Harmonic tracking
    win = int(0.04 * SR)
    hop = win // 2
    h2_1, h3_1, h2_2, h3_2 = [], [], [], []
    times = []
    
    for i in range(0, N - win, hop):
        times.append(i / SR * 1000)
        # Model 1
        spec1 = np.abs(np.fft.rfft(y1[i:i+win] * np.hanning(win))) / (win/2)
        freqs = np.fft.rfftfreq(win, 1/SR)
        h2_1.append(spec1[np.argmin(np.abs(freqs - 2*f0))])
        h3_1.append(spec1[np.argmin(np.abs(freqs - 3*f0))])
        # Model 2
        spec2 = np.abs(np.fft.rfft(y2[i:i+win] * np.hanning(win))) / (win/2)
        h2_2.append(spec2[np.argmin(np.abs(freqs - 2*f0))])
        h3_2.append(spec2[np.argmin(np.abs(freqs - 3*f0))])

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.plot(times, h2_1, color=COLORS[0], alpha=0.3, lw=4, label='Model 1: Even Harmonic (H2)')
    ax.plot(times, h2_2, color=COLORS[1], lw=2, label='Model 2: Even Harmonic (H2)')
    ax.plot(times, h3_1, color='gray', alpha=0.2, lw=4, label='Model 1: Odd Harmonic (H3)')
    ax.plot(times, h3_2, color='black', lw=1, ls=':', label='Model 2: Odd Harmonic (H3)')
    
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Harmonic Amplitude')
    ax.set_title('Dynamic Harmonic Evolution Comparison\nConvergence of spectral behavior between modeling strategies')
    ax.legend(ncol=2, fontsize=9)
    
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_comp2_harmonics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 3: Complex Transient Response ────────────────────────────────────
def fig3_transient():
    # Rapid pick attack then decay
    N = int(SR * 0.3)
    t = np.arange(N)/SR
    env = np.exp(-15 * t) * (np.sin(2*np.pi*2*t)**2) + 0.1
    x = env * np.sin(2*np.pi * 300 * t)
    
    y1, _, _ = model1_coupled(x, 7.0, -0.15, 120, 0.9, 2.0, 40.0, 0.15)
    y2, _, _ = model2_dynamic(x, 5.5, -0.18, 4.0, 1.5, 0.2, 3.0, 80.0)

    fig, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True)
    
    axes[0].plot(t*1000, x, color='black', alpha=0.2, label='Input envelope')
    axes[0].plot(t*1000, y1, color=COLORS[0], lw=0.8, alpha=0.8, label='Model 1 Output')
    axes[0].set_ylabel('Amplitude')
    axes[0].set_title('Model 1: Coupled Response to Transient Input')
    axes[0].legend(loc='upper right')

    axes[1].plot(t*1000, x, color='black', alpha=0.2)
    axes[1].plot(t*1000, y2, color=COLORS[1], lw=0.8, alpha=0.8, label='Model 2 Output')
    axes[1].set_xlabel('Time [ms]')
    axes[1].set_ylabel('Amplitude')
    axes[1].set_title('Model 2: Dynamic Gray-box Response (Touch Sensitivity)')
    axes[1].legend(loc='upper right')

    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_comp3_transient.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


if __name__ == '__main__':
    print("Generating Model 1 vs Model 2 comparison figures...\n")
    fig1_step_sag()
    fig2_harmonics()
    fig3_transient()
    print("\nAll figures saved in:", OUT_DIR)
