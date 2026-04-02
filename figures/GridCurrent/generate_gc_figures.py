"""
Grid Current Model — Scientific Figures
For: "A Physically-Inspired Grid Current Model for Tube Preamp Simulation"
Author: Michel Buffa / Antigravity
"""

import numpy as np
import matplotlib
matplotlib.use('Agg')  # headless rendering
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import MultipleLocator
import os

OUT_DIR = os.path.dirname(os.path.abspath(__file__))
SR = 44100

# ─── Style ────────────────────────────────────────────────────────────────────
plt.rcParams.update({
    'font.family': 'serif',
    'font.size': 11,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'legend.fontsize': 10,
    'figure.dpi': 150,
    'lines.linewidth': 1.8,
    'axes.grid': True,
    'grid.alpha': 0.3,
})
COLORS = ['#1f77b4', '#d62728', '#2ca02c', '#ff7f0e', '#9467bd']

# ─── DSP primitives (mirrors Faust code) ─────────────────────────────────────
def gc_clamp(lo, hi, x): return np.clip(x, lo, hi)

def gc_env_follower(x, tau_ms):
    """RC low-pass envelope follower — tracks rectified grid current."""
    alpha = 1.0 / max(1.0, tau_ms * SR * 0.001)
    alpha = gc_clamp(1e-4, 0.9999, alpha)
    y = np.zeros_like(x)
    y[0] = x[0] * alpha
    for n in range(1, len(x)):
        y[n] = y[n-1] + alpha * (x[n] - y[n-1])
    return y

def grid_current(x, vth):
    """Rectification: current only when signal exceeds threshold."""
    return np.maximum(0.0, x - vth)

def grid_current_bias_block(x, vth, amt_in, amt_bias, tau_ms):
    """Full grid current block: returns (modified_input, bias_shift)."""
    gc = grid_current(x, vth)
    envelope = gc_env_follower(gc, tau_ms)
    in_mod = x - amt_in * envelope
    bias_shift = -(amt_bias * envelope)
    return in_mod, bias_shift, gc, envelope

def triode_stage(x_mod, bias_shift, a, b):
    """Static triode: tanh(a*(x+bias))."""
    return np.tanh(a * x_mod + b + bias_shift)

# ─── Figure 1: Grid Current Characteristic ───────────────────────────────────
def fig1_gc_characteristic():
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))
    x = np.linspace(-1.5, 1.5, 500)

    # Left: rectification for various thresholds
    ax = axes[0]
    for vth, color in zip([0.2, 0.4, 0.6, 0.8], COLORS):
        gc = np.maximum(0.0, x - vth)
        ax.plot(x, gc, color=color, label=f'$V_{{th}}$ = {vth}')
    ax.axhline(0, color='k', lw=0.8, ls='--')
    ax.axvline(0, color='k', lw=0.8, ls='--')
    ax.set_xlabel('Input signal $x$ [normalized]')
    ax.set_ylabel('Grid current $i_g$ [normalized]')
    ax.set_title('(a) Grid current rectification\n$i_g = \\max(0,\\; x - V_{th})$')
    ax.legend()
    ax.set_xlim(-1.5, 1.5)

    # Right: input attenuation effect (amt_in parametric)
    ax = axes[1]
    ax.plot(x, x, 'k--', lw=1.2, label='No GC (bypass)')
    vth = 0.4
    for amt, color in zip([0.1, 0.3, 0.6, 1.0], COLORS):
        gc = np.maximum(0.0, x - vth)
        x_mod = x - amt * gc
        ax.plot(x, x_mod, color=color, label=f'$\\alpha_{{in}}$ = {amt}')
    ax.set_xlabel('Input signal $x$ [normalized]')
    ax.set_ylabel('Modified input $x_{mod}$ [normalized]')
    ax.set_title('(b) Input attenuation\n$x_{mod} = x - \\alpha_{in} \\cdot i_g$  ($V_{th}=0.4$)')
    ax.legend()
    ax.set_xlim(-1.5, 1.5)

    fig.suptitle('Grid Current Model — Nonlinear Characteristic', fontweight='bold', y=1.02)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig1_gc_characteristic.pdf')
    fig.savefig(path, bbox_inches='tight')
    path_png = path.replace('.pdf', '.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Figure 2: RC Envelope Follower Dynamics ─────────────────────────────────
def fig2_envelope_dynamics():
    t = np.arange(SR) / SR  # 1 second
    f = 220.0
    # Burst: strong note, then silence
    burst = np.where(t < 0.3, 1.0, 0.0) * np.sin(2*np.pi*f*t) * 0.9
    gc = np.maximum(0.0, burst - 0.3)  # vth = 0.3

    fig, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

    ax = axes[0]
    ax.plot(t[:int(0.6*SR)], burst[:int(0.6*SR)], color=COLORS[0], lw=1.0, label='Input burst')
    ax.axhline(0.3, color='gray', ls='--', lw=1, label='$V_{th} = 0.3$')
    ax.set_ylabel('Amplitude [normalized]')
    ax.set_title('(a) Input signal with threshold')
    ax.legend()
    ax.set_ylim(-1.1, 1.1)

    ax = axes[1]
    for tau, color in zip([5, 50, 150, 500], COLORS):
        env = gc_env_follower(gc, tau)[:int(0.6*SR)]
        ax.plot(t[:int(0.6*SR)], env, color=color, label=f'$\\tau$ = {tau} ms')
    ax.set_xlabel('Time [s]')
    ax.set_ylabel('Envelope $e(t)$ [normalized]')
    ax.set_title('(b) RC envelope follower response for various $\\tau$\n'
                 '(short $\\tau$ → tight attack, long $\\tau$ → slow "breathing")')
    ax.legend()

    fig.suptitle('Grid Current Envelope Dynamics', fontweight='bold', y=1.01)
    fig.tight_layout()
    path_png = os.path.join(OUT_DIR, 'fig2_envelope_dynamics.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Figure 3: Bias Shift and Operating Point ────────────────────────────────
def fig3_bias_shift():
    x = np.linspace(-2.0, 2.0, 500)
    a, b_static = 4.5, -0.08
    vth, amt_bias = 0.4, 0.3
    gc = np.maximum(0.0, x - vth)

    fig, axes = plt.subplots(1, 2, figsize=(10, 4))

    ax = axes[0]
    for amt, color in zip([0.0, 0.1, 0.3, 0.6], COLORS):
        bias_shift = -(amt * gc)
        y = np.tanh(a * x + b_static + bias_shift)
        label = 'No GC bias' if amt == 0 else f'$\\alpha_{{bias}}$ = {amt}'
        ax.plot(x, y, color=color, label=label)
    ax.set_xlabel('Input $x$')
    ax.set_ylabel('Output $y = \\tanh(a(x + bias))$')
    ax.set_title('(a) Bias shift on transfer function\n(drive $a=4.5$, $V_{th}=0.4$)')
    ax.legend()

    # Right: dynamic snapshot — show how bias shifts for a single transient peak
    ax = axes[1]
    t = np.arange(int(0.05 * SR)) / SR * 1000  # 50ms in ms
    f_note = 200.0
    x_sig = 0.8 * np.sin(2*np.pi*f_note*np.arange(len(t))/SR)
    gc_sig = np.maximum(0.0, x_sig - vth)
    env_sig = gc_env_follower(gc_sig, 80)  # 80ms tau
    bias_dyn = -(amt_bias * env_sig)

    ax.plot(t, x_sig, color=COLORS[0], lw=1.2, label='Input signal')
    ax.plot(t, bias_dyn, color=COLORS[1], lw=1.5, ls='--', label='Dynamic bias shift $\\Delta b(t)$')
    ax.fill_between(t, bias_dyn, alpha=0.15, color=COLORS[1])
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Amplitude [normalized]')
    ax.set_title('(b) Dynamic bias shift over a sustained note\n($A=0.8$, $\\tau=80\\,\\mathrm{ms}$, $\\alpha_{bias}=0.3$)')
    ax.legend()

    fig.suptitle('Grid Current Bias Shift Effect', fontweight='bold', y=1.01)
    fig.tight_layout()
    path_png = os.path.join(OUT_DIR, 'fig3_bias_shift.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Figure 4: Complete Stage Output — With vs Without Grid Current ───────────
def fig4_signal_comparison():
    N = int(0.25 * SR)  # 250ms
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    A = 0.85  # strong playing — above vth
    x = A * np.sin(2*np.pi*f*np.arange(N)/SR)

    a, b = 6.0, -0.12  # Marshall-like
    vth, amt_in, amt_bias, tau = 0.4, 0.3, 0.2, 80.0

    # Without grid current
    y_no_gc = np.tanh(a * x + b)

    # With grid current
    x_mod, bias_shift, gc, env = grid_current_bias_block(x, vth, amt_in, amt_bias, tau)
    y_gc = triode_stage(x_mod, bias_shift, a, b)

    fig, axes = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

    ax = axes[0]
    ax.plot(t_ms, x, color=COLORS[0], lw=1.2, label='Input $x(t)$')
    ax.axhline(vth, color='gray', ls='--', lw=1, label=f'$V_{{th}} = {vth}$')
    ax.set_ylabel('Input [normalized]')
    ax.set_title('(a) Input signal')
    ax.legend(); ax.set_ylim(-1.2, 1.2)

    ax = axes[1]
    ax.plot(t_ms, gc, color=COLORS[1], lw=1.0, label='Grid current $i_g(t)$', alpha=0.7)
    ax.plot(t_ms, env, color=COLORS[2], lw=1.5, ls='--', label='Envelope $e(t)$')
    ax.set_ylabel('Current [normalized]')
    ax.set_title('(b) Grid current and RC envelope')
    ax.legend()

    ax = axes[2]
    ax.plot(t_ms, y_no_gc, color=COLORS[0], lw=1.2, alpha=0.7, label='Without GC')
    ax.plot(t_ms, y_gc, color=COLORS[1], lw=1.5, label='With GC ($\\alpha_{in}=0.3$, $\\alpha_{bias}=0.2$)')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output [normalized]')
    ax.set_title('(c) Stage output — blocking distortion effect')
    ax.legend(); ax.set_ylim(-1.2, 1.2)

    fig.suptitle('Complete Grid Current Model — Signal Processing Chain', fontweight='bold', y=1.01)
    fig.tight_layout()
    path_png = os.path.join(OUT_DIR, 'fig4_signal_comparison.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Figure 5: Harmonic Analysis ─────────────────────────────────────────────
def fig5_harmonic_analysis():
    N = SR * 2  # 2 seconds (steady state)
    f0 = 200.0
    x = 0.85 * np.sin(2*np.pi*f0*np.arange(N)/SR)

    a, b = 6.0, -0.12
    vth, amt_in, amt_bias, tau = 0.4, 0.3, 0.2, 80.0

    y_no_gc = np.tanh(a * x + b)
    x_mod, bias_shift, gc, env = grid_current_bias_block(x, vth, amt_in, amt_bias, tau)
    y_gc = triode_stage(x_mod, bias_shift, a, b)

    # FFT — last 0.5s to get steady state
    win = np.hanning(SR // 2)
    Y_nogc = np.abs(np.fft.rfft(y_no_gc[-SR//2:] * win)) / (SR//4)
    Y_gc   = np.abs(np.fft.rfft(y_gc[-SR//2:]   * win)) / (SR//4)
    freqs  = np.fft.rfftfreq(SR//2, 1/SR)

    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))

    for ax, Y, title, color in zip(
        axes,
        [Y_nogc, Y_gc],
        ['Without Grid Current', 'With Grid Current'],
        [COLORS[0], COLORS[1]]
    ):
        mask = freqs < 5000
        ax.semilogy(freqs[mask], Y[mask] + 1e-10, color=color, lw=1.2)
        # Mark harmonics
        for k in range(1, 15):
            hf = k * f0
            if hf < 5000:
                idx = np.argmin(np.abs(freqs - hf))
                ax.axvline(hf, color='gray', ls=':', lw=0.8, alpha=0.5)
                ax.annotate(f'H{k}', (hf, Y[idx]*1.5), fontsize=7,
                           ha='center', color='gray')
        ax.set_xlabel('Frequency [Hz]')
        ax.set_ylabel('Amplitude [log scale]')
        ax.set_title(title)
        ax.set_xlim(0, 5000)
        ax.set_ylim(1e-6, 1.0)

    fig.suptitle(f'Harmonic Spectrum Comparison (f₀ = {f0:.0f} Hz, A = 0.85)\n'
                 'Grid current enhances even harmonics (H2, H4) — asymmetric "warmth"',
                 fontweight='bold')
    fig.tight_layout()
    path_png = os.path.join(OUT_DIR, 'fig5_harmonic_analysis.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Figure 6: Amplitude Sensitivity (blocking distortion) ───────────────────
def fig6_blocking_distortion():
    """Shows how stronger playing 'strangles' — the grid current attenuates more."""
    N = int(0.5 * SR)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    a, b = 6.0, -0.12
    vth, amt_in, amt_bias, tau = 0.4, 0.3, 0.2, 80.0

    fig, ax = plt.subplots(figsize=(9, 4.5))

    amplitudes = [0.3, 0.5, 0.7, 0.9]
    for A, color in zip(amplitudes, COLORS):
        x = A * np.sin(2*np.pi*f*np.arange(N)/SR)
        x_mod, bias_shift, _, _ = grid_current_bias_block(x, vth, amt_in, amt_bias, tau)
        y = triode_stage(x_mod, bias_shift, a, b)
        # Compute rolling RMS (window 5ms)
        w = int(0.005 * SR)
        rms = np.array([np.sqrt(np.mean(y[max(0,i-w):i+w]**2)) for i in range(0, N, w//2)])
        t_rms = np.arange(len(rms)) * (w//2) / SR * 1000
        ax.plot(t_rms, rms, color=color, label=f'$A = {A}$')

    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output RMS [normalized]')
    ax.set_title('Blocking Distortion — Output RMS vs. Input Amplitude\n'
                 'Stronger playing → grid current → output compressed over time\n'
                 f'($V_{{th}}={vth}$, $\\tau={int(tau)}\\,\\mathrm{{ms}}$, $\\alpha_{{in}}={amt_in}$)')
    ax.legend(title='Input amplitude')
    fig.tight_layout()
    path_png = os.path.join(OUT_DIR, 'fig6_blocking_distortion.png')
    fig.savefig(path_png, bbox_inches='tight')
    print(f"  Saved {path_png}")
    plt.close(fig)

# ─── Main ─────────────────────────────────────────────────────────────────────
if __name__ == '__main__':
    print("Generating scientific figures for grid current model article...\n")
    fig1_gc_characteristic()
    fig2_envelope_dynamics()
    fig3_bias_shift()
    fig4_signal_comparison()
    fig5_harmonic_analysis()
    fig6_blocking_distortion()
    print("\nAll figures saved in:", OUT_DIR)
