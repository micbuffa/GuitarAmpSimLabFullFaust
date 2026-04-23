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

    # Right: varying 'b' (Bias/Operating Point) — Visualizing Asymmetry Break
    ax = axes[1]
    a_f = 4.0
    b_val = -0.6 # Strong bias for visibility
    x_range = np.linspace(-1.5, 1.5, 500)
    
    y_sym = np.tanh(a_f * x_range)
    y_bi  = np.tanh(a_f * x_range + b_val)
    
    ax.plot(x_range, y_sym, 'k--', lw=1, alpha=0.3, label='Symmetric ($b=0$)')
    ax.plot(x_range, y_bi, color=COLORS[1], lw=2, label=f'Biased ($b={b_val}$)')
    
    # Highlight the Operating Point
    op_y = np.tanh(b_val)
    ax.plot(0, op_y, 'ro', markersize=6, label='Operating Point')
    
    # Show Asymmetric Headroom for input range [-1, 1]
    y_pos = np.tanh(a_f * 1.0 + b_val)
    y_neg = np.tanh(a_f * -1.0 + b_val)
    
    ax.annotate('', xy=(1.0, y_pos), xytext=(1.0, op_y), arrowprops=dict(arrowstyle='<->', color='green', lw=1.5))
    ax.text(1.05, (y_pos+op_y)/2, 'Pos. Headroom', rotation=90, va='center', color='green', fontsize=8)
    
    ax.annotate('', xy=(-1.0, y_neg), xytext=(-1.0, op_y), arrowprops=dict(arrowstyle='<->', color='blue', lw=1.5))
    ax.text(-1.15, (y_neg+op_y)/2, 'Neg. Headroom', rotation=90, va='center', color='blue', fontsize=8)

    ax.axhline(0, color='k', lw=0.5, ls='-')
    ax.axvline(0, color='k', lw=0.5, ls='-')
    
    ax.set_xlabel('Input $x$')
    ax.set_ylabel('Output $y$')
    ax.set_title('(b) Bias-Induced Asymmetry\nUnequal distance to saturation for $\\pm$ excursions')
    ax.legend(loc='lower right', fontsize=8)
    ax.set_xlim(-1.5, 1.5)

    fig.suptitle('Static Triode Model — Transfer Characteristics', fontweight='bold', y=1.02)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_triode1_transfer.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: Waveform Distortion — The Symmetry Break ───────────────────────
def fig2_waveform_distortion():
    N = int(SR * 0.005)  # 5ms window
    t_ms = np.arange(N) / SR * 1000
    f = 300.0
    # Input sine: strong to force deep clipping
    x = 2.0 * np.sin(2*np.pi * f * np.arange(N)/SR) 

    fig, axes = plt.subplots(1, 2, figsize=(11, 5))

    # Left: Symmetry Mirror (Irreproachable evidence of asymmetry)
    ax = axes[0]
    # a=1.0, b=-1.1 with x=2.2 means:
    # Top Peak u = 1(2.2) - 1.1 = 1.1 -> tanh(1.1) = 0.8 (Very soft/round)
    # Bot Peak u = 1(-2.2) - 1.1 = -3.3 -> tanh(-3.3) = -0.997 (Rock flat)
    a, b = 1.0, -1.1 
    y = triode_static(x, a, b)
    
    h_cycle = int(0.5 * SR / f)
    pos_segment = y[0:h_cycle]
    neg_segment = y[h_cycle:2*h_cycle]
    t_seg = t_ms[0:h_cycle]
    
    ax.plot(t_ms, y, color=COLORS[1], lw=2.5, label='Biased Output Signal', alpha=0.9)
    
    # THE MIRROR: Overlay inverted positive cycle on the negative cycle
    ax.plot(t_seg + 1000/(2*f), -pos_segment, color=COLORS[0], lw=2.0, ls='--', 
            label='Mirrored Positive Peak')
    
    # Shade the difference (The "Asymmetry Gap")
    ax.fill_between(t_seg + 1000/(2*f), -pos_segment, neg_segment, color='purple', alpha=0.3, label='Asymmetry Gap')

    ax.annotate('Soft "Breathing" Peak\n(Rounded top)', xy=(0.83, 0.78), xytext=(1.1, 1.1),
                arrowprops=dict(arrowstyle='->', color='green', lw=1.5))
    ax.annotate('Hard Compressed Peak\n(Flat plateau)', xy=(2.5, -0.99), xytext=(3.2, -1.25),
                arrowprops=dict(arrowstyle='->', color='red', lw=1.5))

    ax.set_ylim(-1.4, 1.4)
    ax.axhline(0, color='k', lw=1.0)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Amplitude')
    ax.set_title('(a) Contrast Analysis: Round vs. Flat Peaks\nTop peak is preserved; Bottom peak "hits the wall" early')
    ax.legend(loc='lower left', fontsize=8)

    # Right: Saturation Headroom Detail (Scaling fixed)
    ax = axes[1]
    zoom_n = int(0.0035 * SR)
    t_z = np.arange(zoom_n) / SR * 1000
    x_ref = 1.0 * np.sin(2*np.pi * f * (np.arange(zoom_n)/SR)) # Scaled down for framing
    
    y_z = triode_static(1.5 * x_ref, a, b) # High drive inner signal
    
    ax.plot(t_z, x_ref, 'k--', lw=1, alpha=0.4, label='Input Sine (Ref)')
    ax.plot(t_z, y_z, color=COLORS[1], lw=2.0, label='Biased Output')
    
    # Shading the headroom imbalance
    ax.fill_between(t_z, x_ref, y_z, where=(x_ref>y_z)&(x_ref>0), color='green', alpha=0.15, label='Soft Clipping')
    ax.fill_between(t_z, x_ref, y_z, where=(y_z<x_ref)&(x_ref<0), color='red', alpha=0.25, label='Hard Clipping')
    
    ax.set_xlabel('Time [ms]')
    ax.set_title('(b) Dynamic Compression Imbalance\nRed area: Instant saturation / Green: Soft rounding')
    ax.legend(loc='lower right', fontsize=8)
    ax.set_ylim(-1.4, 1.4)
    ax.axhline(0, color='k', lw=0.5)

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
