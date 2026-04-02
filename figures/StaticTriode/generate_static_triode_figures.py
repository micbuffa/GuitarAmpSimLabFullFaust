"""
Static Triode Model — Scientific Figures
For: "Real-time Tanh-based Triode Simulation with ADAA1 Anti-Aliasing"
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

def triode_static(x, a, b):
    return np.tanh(a * x + b)

# ADAA1 simplified simulation
def logcosh(x):
    return np.abs(x) - np.log(2.0) + np.log(1.0 + np.exp(-2.0 * np.abs(x)))

def adaa1_tanh(x, x_prev):
    diff = x - x_prev
    if np.abs(diff) < 1e-5:
        return np.tanh(x)
    return (logcosh(x) - logcosh(x_prev)) / diff


# ─── Figure 1: Transfer Function Curves ──────────────────────────────────────
def fig1_transfer_function():
    x = np.linspace(-1.5, 1.5, 500)
    fig, axes = plt.subplots(1, 2, figsize=(10, 4))

    # Left: varying 'a' (Drive/Gain)
    ax = axes[0]
    for a, color in zip([1.5, 3.0, 6.0, 10.0], COLORS):
        y = triode_static(x, a, 0)
        ax.plot(x, y, color=color, label=f'$a = {a}$')
    ax.set_xlabel('Input $x$')
    ax.set_ylabel('Output $y = \\tanh(a \\cdot x)$')
    ax.set_title('(a) Varying Drive ($a$)\nIncreasing $a$ narrows the linear region')
    ax.legend()

    # Right: varying 'b' (Bias/Operating Point)
    ax = axes[1]
    a_fixed = 5.0
    for b, color in zip([0.0, -0.1, -0.2, -0.4], COLORS):
        y = triode_static(x, a_fixed, b)
        ax.plot(x, y, color=color, label=f'$b = {b}$')
    ax.set_xlabel('Input $x$')
    ax.set_ylabel('Output $y = \\tanh(a \\cdot x + b)$')
    ax.set_title('(b) Varying Bias ($b$)\n$b \\neq 0$ introduces asymmetry')
    ax.legend()

    fig.suptitle('Static Triode Model — Transfer Characteristics', fontweight='bold', y=1.02)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_triode1_transfer.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: Waveform Distortion ───────────────────────────────────────────
def fig2_waveform_distortion():
    N = int(SR * 0.01)  # 10ms
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    x = np.sin(2*np.pi * f * np.arange(N)/SR)

    fig, axes = plt.subplots(1, 2, figsize=(10, 4.5))

    # Left: Symmetrical vs Asymmetrical (Clipping character)
    ax = axes[0]
    ax.plot(t_ms, x, 'k--', lw=1.0, alpha=0.5, label='Input sine')
    y_sym = triode_static(x, 6.0, 0.0)
    y_asym = triode_static(x, 6.0, -0.25)
    ax.plot(t_ms, y_sym, color=COLORS[0], label='Symmetrical ($b=0$)')
    ax.plot(t_ms, y_asym, color=COLORS[1], label='Asymmetrical ($b=-0.25$)')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Amplitude')
    ax.set_title('(a) Symmetrical vs. Asymmetrical Clipping')
    ax.legend(loc='upper right', fontsize=9)

    # Right: Soft vs Hard onset (Drive effect)
    ax = axes[1]
    for a, color in zip([2, 5, 12], COLORS[2:]):
        y = triode_static(x, a, 0)
        ax.plot(t_ms, y, color=color, label=f'$a = {a}$')
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Amplitude')
    ax.set_title('(b) Progressive Saturation Enclosure')
    ax.legend(loc='upper right', fontsize=9)

    fig.suptitle('Waveform Deformation — Non-linear Effects', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_triode2_waveform.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 3: Harmonic Analysis ─────────────────────────────────────────────
def fig3_harmonic_analysis():
    N = SR // 2
    f0 = 200.0
    x = 0.8 * np.sin(2*np.pi * f0 * np.arange(N)/SR)

    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5), sharey=True)

    cases = [
        {'a': 6.0, 'b': 0.0,   'title': 'Symmetrical ($b=0$) -> Odd Harmonics', 'color': COLORS[0]},
        {'a': 6.0, 'b': -0.25, 'title': 'Asymmetrical ($b=-0.25$) -> Even Harmonics', 'color': COLORS[1]}
    ]

    for ax, case in zip(axes, cases):
        y = triode_static(x, case['a'], case['b'])
        win = np.hanning(N)
        Y = np.abs(np.fft.rfft(y * win)) / (N/2)
        freqs = np.fft.rfftfreq(N, 1/SR)

        mask = (freqs > 100) & (freqs < 4000)
        ax.semilogy(freqs[mask], Y[mask] + 1e-10, color=case['color'])

        # Mark harmonics
        for k in range(1, 15):
            hf = k * f0
            if hf < 4000:
                idx = np.argmin(np.abs(freqs - hf))
                ax.axvline(hf, color='gray', ls=':', lw=0.8, alpha=0.5)
                ax.annotate(f'H{k}', (hf, Y[idx]*1.5), fontsize=7, ha='center', color='gray')

        ax.set_xlabel('Frequency [Hz]')
        ax.set_title(case['title'])
        ax.set_ylim(1e-6, 1.0)

    axes[0].set_ylabel('Amplitude [log]')
    fig.suptitle('Harmonic Content — Effect of DC Bias on Symmetry', fontweight='bold')
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_triode3_harmonics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 4: ADAA1 Aliasing Reduction ──────────────────────────────────────
def fig4_adaa_analysis():
    # To see aliasing, use a high freq sine near Nyquist/4 or sweep
    f_aliasing = 12000.0
    N = SR // 10
    t = np.arange(N)/SR
    x = 0.95 * np.sin(2*np.pi * f_aliasing * t)

    # Standard tanh
    y_std = np.tanh(10 * x)

    # ADAA1 tanh
    y_adaa = np.zeros_like(x)
    x_prev = 0
    for i in range(len(x)):
        y_adaa[i] = adaa1_tanh(10 * x[i], 10 * x_prev)
        x_prev = x[i]

    win = np.hanning(N)
    Y_std = np.abs(np.fft.rfft(y_std * win)) / (N/2)
    Y_ada = np.abs(np.fft.rfft(y_adaa * win)) / (N/2)
    freq_axis = np.fft.rfftfreq(N, 1/SR)

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.semilogy(freq_axis, Y_std + 1e-10, color='gray', alpha=0.5, label='Standard Tanh (Aliasing present)')
    ax.semilogy(freq_axis, Y_ada + 1e-10, color=COLORS[2], label='ADAA1 (Antiderivative Anti-Aliasing)')

    # Identify mirrored alien components
    # f_harmonic = k*f, alias = |n*SR - f_harmonic|
    ax.set_xlim(0, SR/2)
    ax.set_ylim(1e-7, 1.0)
    ax.set_xlabel('Frequency [Hz]')
    ax.set_ylabel('Amplitude [log]')
    ax.set_title('Aliasing Suppression via ADAA1\nInput: $12\\,\\mathrm{kHz}$ Sine at $f_s = 44.1\\,\\mathrm{kHz}$')
    ax.legend()

    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_triode4_adaa.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


if __name__ == '__main__':
    print("Generating static triode model figures...\n")
    fig1_transfer_function()
    fig2_waveform_distortion()
    fig3_harmonic_analysis()
    fig4_adaa_analysis()
    print("\nAll figures saved in:", OUT_DIR)
