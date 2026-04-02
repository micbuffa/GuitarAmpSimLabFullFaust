"""
Cathode Bias Model — Scientific Figures
For: "A Physically-Inspired Cathode Bias Model for Tube Preamp Simulation"
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
    'lines.linewidth': 1.8,
    'axes.grid': True,
    'grid.alpha': 0.3,
})
COLORS = ['#1f77b4', '#d62728', '#2ca02c', '#ff7f0e', '#9467bd']


# ─── DSP primitives ──────────────────────────────────────────────────────────

def iir_lowpass(x, alpha):
    """First-order IIR lowpass (si.smooth equivalent)."""
    y = np.zeros_like(x)
    y[0] = x[0] * alpha
    for n in range(1, len(x)):
        y[n] = y[n-1] + alpha * (x[n] - y[n-1])
    return y

def cathode_shift(x, tau_ms, scale):
    """
    Faust: cathode_shift(tau, scale, x) =
        x : max(0.0) : si.smooth(1/max(1, tau*SR*0.001)) : *(scale)
    """
    rect = np.maximum(0.0, x)
    alpha = 1.0 / max(1.0, tau_ms * SR * 0.001)
    env = iir_lowpass(rect, alpha)
    return env * scale

def triode_stage(x, a, b, cath_bias):
    return np.tanh(a * x + b - cath_bias)


# ─── Figure 1: Cathode Shift Characteristic ──────────────────────────────────
def fig1_cathode_characteristic():
    x = np.linspace(-1.5, 1.5, 500)
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))

    # Left: half-wave rectification
    ax = axes[0]
    rect = np.maximum(0.0, x)
    ax.plot(x, x, 'k--', lw=1.2, label='Input $x$')
    ax.plot(x, rect, color=COLORS[0], lw=2, label='Rectified: $\\max(0, x)$')
    ax.fill_between(x, rect, alpha=0.15, color=COLORS[0])
    ax.axhline(0, color='gray', lw=0.8)
    ax.axvline(0, color='gray', lw=0.8)
    ax.set_xlabel('Input signal $x$ [normalized]')
    ax.set_ylabel('Output [normalized]')
    ax.set_title('(a) Half-wave rectification\nOnly positive peaks charge the cathode capacitor')
    ax.legend()

    # Right: static shift effect on tanh transfer function
    ax = axes[1]
    a, b = 5.0, -0.10
    ax.plot(x, np.tanh(a*x + b), 'k--', lw=1.5, label='No cathode bias (scale=0)')
    for scale, color in zip([0.05, 0.10, 0.20, 0.35], COLORS):
        # Bias builds for positive input — approximate steady-state cathode voltage
        # as scale * mean(max(0, x)) for x ~ 0.5 peak
        cb_approx = scale * 0.25  # approximate steady-state bias for A=0.5 signal
        y = np.tanh(a*x + b - cb_approx)
        ax.plot(x, y, color=color, label=f'scale = {scale}  ($\\Delta b \\approx {cb_approx:.3f}$)')
    ax.set_xlabel('Input signal $x$ [normalized]')
    ax.set_ylabel('Stage output $y$')
    ax.set_title('(b) Operating point shift on tanh\n(static approximation, drive $a=5$)')
    ax.legend(fontsize=9)

    fig.suptitle('Cathode Bias Model — Core Functions', fontweight='bold', y=1.02)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb1_characteristic.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: RC Time Constant Dynamics ─────────────────────────────────────
def fig2_rc_dynamics():
    N = int(SR * 1.2)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    A = 0.7
    # Sustained note then silence
    env_sig = np.where(t_ms < 600, A * np.sin(2*np.pi*f*np.arange(N)/SR), 0.0)

    fig, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True)

    ax = axes[0]
    ax.plot(t_ms, env_sig, color=COLORS[0], lw=0.8, alpha=0.7, label='Input signal $x(t)$')
    ax.axvline(600, color='gray', ls='--', lw=1.2, label='Note off')
    ax.set_ylabel('Amplitude')
    ax.set_title('(a) Input signal: sustained note followed by silence')
    ax.legend()

    ax = axes[1]
    for tau, color in zip([10, 40, 100, 250], COLORS):
        cb = cathode_shift(env_sig, tau, scale=0.15)
        ax.plot(t_ms, cb, color=color, label=f'$\\tau = {tau}$ ms (e.g. $R_k C_k$)')
    ax.axvline(600, color='gray', ls='--', lw=1.2)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Cathode bias $\\Delta b(t)$')
    ax.set_title('(b) Cathode bias build-up and decay for various RC time constants\n'
                 '(scale = 0.15 for all)')
    ax.legend()

    fig.suptitle('Cathode Bias Dynamics — RC Time Constant Effect', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb2_rc_dynamics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 3: Scale Parameter — Operating Point Shift ───────────────────────
def fig3_scale_effect():
    N = int(SR * 0.4)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    A = 0.7
    x = A * np.sin(2*np.pi*f*np.arange(N)/SR)
    a, b = 5.0, -0.10
    tau = 50.0

    fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))

    # Left: output waveform for different scales
    ax = axes[0]
    ax.plot(t_ms, np.tanh(a*x + b), 'k--', lw=1.2, alpha=0.7, label='scale=0 (no CB)')
    for scale, color in zip([0.05, 0.15, 0.30, 0.45], COLORS):
        cb = cathode_shift(x, tau, scale)
        y = triode_stage(x, a, b, cb)
        ax.plot(t_ms, y, color=color, lw=1.2, label=f'scale = {scale}')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output [normalized]')
    ax.set_title(f'(a) Stage output vs. scale\n($\\tau={int(tau)}$ ms, $a={a}$, $A={A}$)')
    ax.legend(fontsize=9)

    # Right: output RMS vs time to show progressive compression
    ax = axes[1]
    w = int(0.005 * SR)  # 5ms window RMS
    step = w // 2

    # Baseline (scale=0)
    y_baseline = np.tanh(a*x + b)
    rms_baseline = [np.sqrt(np.mean(y_baseline[max(0, i-w):i+w]**2)) for i in range(0, N, step)]
    t_rms = np.arange(len(rms_baseline)) * step / SR * 1000
    ax.plot(t_rms, rms_baseline, 'k--', lw=1.2, label='scale=0')

    for scale, color in zip([0.05, 0.15, 0.30, 0.45], COLORS):
        cb = cathode_shift(x, tau, scale)
        y = triode_stage(x, a, b, cb)
        rms = [np.sqrt(np.mean(y[max(0, i-w):i+w]**2)) for i in range(0, N, step)]
        ax.plot(t_rms, rms, color=color, label=f'scale = {scale}')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output RMS [normalized]')
    ax.set_title(f'(b) Progressive compression (RMS)\n($\\tau={int(tau)}$ ms, $A={A}$)')
    ax.legend(fontsize=9)

    fig.suptitle('Cathode Bias — Scale Parameter Effect', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb3_scale_effect.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 4: Harmonic Spectrum With vs Without Cathode Bias ─────────────────
def fig4_harmonic_spectrum():
    N_total = SR * 3
    f0 = 200.0
    A = 0.7
    x = A * np.sin(2*np.pi*f0*np.arange(N_total)/SR)
    a, b = 5.0, -0.10
    tau, scale = 50.0, 0.20

    y_no_cb = np.tanh(a*x + b)
    cb = cathode_shift(x, tau, scale)
    y_cb = triode_stage(x, a, b, cb)

    # Use last second (steady state)
    seg = slice(-SR, None)
    win = np.hanning(SR)
    Y_no = np.abs(np.fft.rfft(y_no_cb[seg] * win)) / (SR/2)
    Y_cb = np.abs(np.fft.rfft(y_cb[seg]   * win)) / (SR/2)
    freqs = np.fft.rfftfreq(SR, 1/SR)

    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))
    for ax, Y, title, color in zip(axes,
            [Y_no, Y_cb],
            ['Without Cathode Bias', f'With Cathode Bias (scale={scale}, τ={int(tau)} ms)'],
            [COLORS[0], COLORS[1]]):
        mask = freqs < 5000
        ax.semilogy(freqs[mask], Y[mask] + 1e-10, color=color, lw=1.2)
        for k in range(1, 15):
            hf = k * f0
            if hf < 5000:
                idx = np.argmin(np.abs(freqs - hf))
                ax.axvline(hf, color='gray', ls=':', lw=0.8, alpha=0.5)
                ax.annotate(f'H{k}', (hf, Y[idx]*1.5), fontsize=7, ha='center', color='gray')
        ax.set_xlabel('Frequency [Hz]')
        ax.set_ylabel('Amplitude [log]')
        ax.set_title(title)
        ax.set_xlim(0, 5000)
        ax.set_ylim(1e-6, 1.0)

    fig.suptitle(f'Harmonic Spectrum — Cathode Bias Effect (f₀={f0:.0f} Hz, A={A})',
                 fontweight='bold')
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb4_harmonics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 5: Preset Comparison (Marshall vs Fender vs Peavey) ───────────────
def fig5_preset_comparison():
    # Preset cathode bias parameters
    presets = {
        'Fender Twin (clean)':  dict(tau=40.0,  scale=0.08,  color=COLORS[0]),
        'Marshall JCM800':      dict(tau=68.0,  scale=0.12,  color=COLORS[1]),
        'Vox AC30 (Class A)':   dict(tau=60.0,  scale=0.18,  color=COLORS[2]),
        'Peavey 5150 (hi-gain)':dict(tau=15.0,  scale=0.20,  color=COLORS[3]),
    }
    N = int(SR * 0.8)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    A = 0.75
    x = A * np.sin(2*np.pi*f*np.arange(N)/SR)
    a, b = 6.0, -0.12

    fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

    ax = axes[0]
    for name, p in presets.items():
        cb = cathode_shift(x, p['tau'], p['scale'])
        ax.plot(t_ms, cb, color=p['color'], lw=1.5, label=name)
    ax.set_ylabel('Cathode bias $\\Delta b(t)$')
    ax.set_title('(a) Cathode bias trajectory — preset comparison')
    ax.legend()

    ax = axes[1]
    ax.plot(t_ms, np.tanh(a*x+b), 'k--', lw=1.0, alpha=0.5, label='No cathode bias')
    for name, p in presets.items():
        cb = cathode_shift(x, p['tau'], p['scale'])
        y = triode_stage(x, a, b, cb)
        ax.plot(t_ms, y, color=p['color'], lw=1.2, alpha=0.85, label=name)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Stage output [normalized]')
    ax.set_title('(b) Resulting output signal — dynamic character per amplifier')
    ax.legend(fontsize=9)
    ax.set_ylim(-1.1, 1.1)

    fig.suptitle('Cathode Bias Model — Amplifier Preset Comparison', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb5_preset_comparison.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 6: Multi-Stage Accumulation ──────────────────────────────────────
def fig6_multistage():
    """Shows how cathode bias accumulates across the 3-stage chain."""
    N = int(SR * 0.3)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    A = 0.80
    x_in = A * np.sin(2*np.pi*f*np.arange(N)/SR)

    # Marshall preset: 3 stages
    params = [
        dict(a=6.0, b=-0.12, tau=68.0, scale=0.12),  # Stage 1
        dict(a=5.0, b=-0.16, tau=25.0, scale=0.20),  # Stage 2
        dict(a=3.5, b=-0.15, tau=12.0, scale=0.22),  # Stage 3 (driver)
    ]
    gk = 0.43 * 0.07  # Marshall-calibrated interstage gain

    fig, axes = plt.subplots(len(params)+1, 1, figsize=(10, 9), sharex=True)
    axes[0].plot(t_ms, x_in, color='k', lw=1.2)
    axes[0].set_ylabel('Amplitude')
    axes[0].set_title('Input signal')

    x = x_in.copy()
    for i, p in enumerate(params):
        cb = cathode_shift(x, p['tau'], p['scale'])
        y = triode_stage(x, p['a'], p['b'], cb)
        ax = axes[i+1]
        ax.plot(t_ms, y, color=COLORS[i], lw=1.2, label='Output')
        ax.plot(t_ms, -cb, color=COLORS[i], lw=1.0, ls='--', alpha=0.7,
                label=f'$-\\Delta b(t)$ (τ={p["tau"]:.0f} ms, scale={p["scale"]})')
        ax.set_ylabel('Output [norm.]')
        ax.set_title(f'Stage {i+1}  (a={p["a"]}, b={p["b"]})')
        ax.legend(fontsize=9)
        x = y * (gk if i < 2 else 1.0)

    axes[-1].set_xlabel('Time [ms]')
    fig.suptitle('Cathode Bias — Three-Stage Chain (Marshall JCM800 preset)\n'
                 'Each stage accumulates its own cathode voltage; progressive compression',
                 fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb6_multistage.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


if __name__ == '__main__':
    print("Generating cathode bias figures...\n")
    fig1_cathode_characteristic()
    fig2_rc_dynamics()
    fig3_scale_effect()
    fig4_harmonic_spectrum()
    fig5_preset_comparison()
    fig6_multistage()
    print("\nAll figures saved in:", OUT_DIR)
