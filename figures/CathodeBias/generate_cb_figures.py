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

    # Right: Operating Point Migration via 'scale' parameter
    ax = axes[1]
    a_m, b_m = 4.0, -0.1
    x_grid = np.linspace(-1.5, 1.5, 500)
    
    # Baseline
    y_base = np.tanh(a_m*x_grid + b_m)
    ax.plot(x_grid, y_base, 'k--', lw=1.2, alpha=0.4, label='Baseline (scale=0)')
    
    scales = [0.2, 0.5, 0.8]
    op_points = []
    
    # Show migration for various scale values
    for scale, color in zip(scales, COLORS[1:]):
        # We simulate the steady-state bias shift induced by a full-scale signal
        cb_shift = scale * 0.5 
        y = np.tanh(a_m * x_grid + b_m - cb_shift)
        ax.plot(x_grid, y, color=color, lw=2.2, label=f'scale = {scale}')
        
        # Mark the shifted operating point at x=0
        op_y = np.tanh(b_m - cb_shift)
        op_points.append((0, op_y))
        ax.plot(0, op_y, 'o', color=color, markersize=8, markeredgecolor='white', zorder=5)
        
        # Add a Gain Sag projection to Y axis
        ax.hlines(op_y, -0.1, 0.1, colors=color, linestyles='-', lw=1.5)
        ax.annotate(f'Sag {op_y:.2f}', xy=(0, op_y), xytext=(-0.6, op_y),
                    arrowprops=dict(arrowstyle='->', color=color, alpha=0.6),
                    fontsize=8, color=color, va='center')
        
    # Draw the Migration Path
    p_x = [p[0] for p in op_points]
    p_y = [p[1] for p in op_points]
    ax.plot(p_x, p_y, color='black', lw=1.0, ls=':', alpha=0.6, zorder=4)

    # Migration Arrow
    ax.annotate('Migration Path\n(Gain Sag)', xy=(0, -0.5), xytext=(0.3, -0.8),
                arrowprops=dict(arrowstyle='->', color='black', lw=1.5, connectionstyle='arc3,rad=.2'),
                ha='left', fontsize=9, fontweight='bold')

    ax.axvline(0, color='gray', lw=1.2, ls='-', alpha=0.3)
    ax.set_xlabel('Input signal $x$')
    ax.set_ylabel('Output $y$')
    ax.set_title('(b) Operating Point Migration\n"Scale" determines the depth of the drift into the "Sag" region')
    ax.legend(loc='lower right', fontsize=8)
    ax.set_ylim(-1.1, 1.1)

    fig.suptitle('Cathode Bias Model — Core Functions', fontweight='bold', y=1.02)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb1_characteristic.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 2: RC Time Constant Dynamics ─────────────────────────────────────
def fig2_rc_dynamics():
    N = int(SR * 1.5)
    t_ms = np.arange(N) / SR * 1000
    f = 200.0
    
    # Rhythmic pulse sequence: 3 notes with different attack/intensity
    x = np.zeros(N)
    x[0:int(0.4*SR)] = 0.8 * np.sin(2*np.pi*f*np.arange(int(0.4*SR))/SR)
    x[int(0.5*SR):int(0.6*SR)] = 0.5 * np.sin(2*np.pi*f*np.arange(int(0.1*SR))/SR)
    x[int(0.8*SR):int(1.3*SR)] = 0.9 * np.sin(2*np.pi*f*np.arange(int(0.5*SR))/SR) * np.linspace(1, 0.4, int(0.5*SR))

    fig, axes = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

    # (a) Input vs Dynamic Gain
    ax = axes[0]
    ax.plot(t_ms, x, color='black', lw=0.6, alpha=0.3, label='Input Signal (Guitar Strike)')
    
    tau_ref = 80.0
    cb_ref = cathode_shift(x, tau_ref, scale=0.45)
    gain_env = 1.0 - cb_ref
    
    ax.plot(t_ms, gain_env, color=COLORS[1], lw=2.0, label=f'Effective Gain (Sag, $\\tau={int(tau_ref)}$ms)')
    ax.fill_between(t_ms, 1.0, gain_env, color=COLORS[1], alpha=0.15, label='Gain Reduction Area')
    
    ax.annotate('Attack Compression', xy=(100, 0.9), xytext=(150, 1.1),
                arrowprops=dict(arrowstyle='->', color='black'))
    ax.annotate('Note Rebound', xy=(650, 0.98), xytext=(650, 1.1),
                arrowprops=dict(arrowstyle='->', color='black'))

    ax.set_ylim(-1.0, 1.3)
    ax.set_ylabel('Amplitude / Gain')
    ax.set_title('(a) Rhythmic Input and Resulting Sag Influence\nThe gain "compresses" during loud strikes and "breathes" during pauses')
    ax.legend(loc='lower right', fontsize=9)

    # (b) Comparison of recovery times
    ax = axes[1]
    for tau, color, label in zip([25, 120, 400], [COLORS[0], COLORS[1], '#2ca02c'], 
                                 ['Tight (Modern HI-GAIN)', 'Medium (Classic ROCK)', 'Spongy (Vintage TWEED)']):
        cb = cathode_shift(x, tau, scale=0.4)
        ax.plot(t_ms, cb, color=color, lw=1.8, label=f'$\\tau = {tau}$ ms — {label}')
    
    ax.annotate('Fast tracking', xy=(200, 0.2), xytext=(50, 0.35),
                arrowprops=dict(arrowstyle='->', color=COLORS[0]))
    ax.annotate('Slow build-up (Holistic sag)', xy=(1000, 0.15), xytext=(1000, 0.4),
                arrowprops=dict(arrowstyle='->', color='#2ca02c'), ha='center')

    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Cathode bias $\\Delta b(t)$')
    ax.set_title('(b) Cathode Bias Evolution for Different Topologies\nHow the amp "forgets" or "remembers" the previous strike')
    ax.legend(loc='upper right', fontsize=9)
    ax.set_ylim(0, 0.6)

    fig.suptitle('Cathode Bias Dynamics — RC Time Constant Effect', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb2_rc_dynamics.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 3: Scale Parameter — Operating Point Shift ───────────────────────
def fig3_scale_effect():
    N = int(SR * 0.7)
    t_ms = np.arange(N) / SR * 1000
    f = 120.0 
    A = 1.2
    x = A * np.sin(2*np.pi * f * np.arange(N)/SR)
    
    a, b = 2.5, -0.2
    tau = 40.0

    fig, axes = plt.subplots(1, 2, figsize=(11, 5))

    # Left: Waveform Asymmetry Detail
    ax = axes[0]
    zoom_start = int(0.6 * SR)
    zoom_end = zoom_start + int(0.015 * SR)
    tz = t_ms[zoom_start:zoom_end]
    xz = x[zoom_start:zoom_end]
    
    y0 = np.tanh(a * xz + b)
    ax.plot(tz, y0, 'k--', lw=1.5, alpha=0.6, label='Baseline (scale=0)')
    
    scale_ex = 4.5
    cb = cathode_shift(x, tau, scale_ex)
    y_ex = triode_stage(xz, a, b, cb[zoom_start:zoom_end])
    
    ax.plot(tz, y_ex, color='#d62728', lw=3.0, label=f'Extreme Sag (scale={scale_ex})')
    ax.fill_between(tz, y0, y_ex, color='red', alpha=0.2, label='Energy sag (Headroom loss)')

    ax.annotate('Positive Headroom Crushed\n(The "Choke")', xy=(604.5, 0.45), xytext=(604, 1.1),
                arrowprops=dict(arrowstyle='->', color='red', lw=2), ha='center', fontsize=9, fontweight='bold')

    ax.set_ylim(-1.6, 1.6)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Output Amplitude')
    ax.set_title('(a) Contrast Analysis: Dynamic Headroom\nEffect of extreme "scale" on the output waveform')
    ax.legend(loc='lower left', fontsize=8)

    # Right: Auditory Energy Drop (AC-RMS)
    ax = axes[1]
    w = int(0.05 * SR) 
    step = w // 2
    
    scales = [0.0, 1.5, 4.5]
    colors = ['black', '#ff7f0e', '#d62728']
    labels = ['No Sag (Symmetric)', 'Moderate Sag (Vintage)', 'Extreme Sag (Choked)']
    
    for scale, color, label in zip(scales, colors, labels):
        cb = cathode_shift(x, tau, scale)
        y = triode_stage(x, a, b, cb)
        
        rms_ac = []
        for i in range(0, N - w, step):
            win = y[i:i+w]
            win_ac = win - np.mean(win)
            rms_ac.append(np.sqrt(np.mean(win_ac**2)))
            
        tr = np.arange(len(rms_ac)) * step / SR * 1000
        ls = '--' if scale == 0 else '-'
        ax.plot(tr, rms_ac, color=color, ls=ls, lw=2.5, label=f'scale = {scale} — {label}')
        
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Audible Energy (AC-RMS)')
    ax.set_title('(b) Flagrant Volume Drop / Sag\nAudible energy "chokes" over time as bias builds up')
    ax.legend(loc='lower left', fontsize=8)
    ax.set_ylim(0.2, 0.9)
    
    ax.annotate('Flagrant Drop\n(Audible Sag)', xy=(450, 0.45), xytext=(200, 0.3),
                arrowprops=dict(arrowstyle='->', color='red', lw=2), fontweight='bold', fontsize=10)

    fig.suptitle('Cathode Bias — Scale Parameter Effect', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb3_scale_effect.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 4: Harmonic Spectrum ─────────────────────────────────────────────
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


# ─── Figure 5: Amplifier Preset Comparison (Premium/Dense) ───────────────────
def fig5_preset_comparison():
    N = int(SR * 1.3)
    t_ms = np.arange(N) / SR * 1000
    f = 140.0
    x = np.zeros(N)
    # Gallop pulses + long sustain
    pulses = [(0.0, 0.15), (0.2, 0.35), (0.4, 0.55), (0.6, 0.75), (0.8, 1.25)]
    for start, end in pulses:
        n = int((end - start) * SR)
        s = int(start * SR)
        x[s:s+n] = 0.9 * np.sin(2*np.pi * f * np.arange(n)/SR)
    
    presets = [
        {'name': 'Modern HI-GAIN (5150)', 'tau': 18.0,  'scale': 0.65, 'color': COLORS[3]},
        {'name': 'Vintage CLEAN (Twin)',  'tau': 150.0, 'scale': 0.35, 'color': COLORS[0]},
        {'name': 'Class A BREATH (AC30)', 'tau': 400.0, 'scale': 0.85, 'color': COLORS[2]},
        {'name': 'Crunch DRIVE (JCM800)', 'tau': 90.0,  'scale': 0.50, 'color': COLORS[1]}
    ]

    fig, axes = plt.subplots(2, 1, figsize=(10, 8), sharex=True)
    ax0, ax1 = axes[0], axes[1]

    for p in presets:
        cb = cathode_shift(x, p['tau'], p['scale'])
        gain_env = 1.0 - cb
        ax0.plot(t_ms, cb, color=p['color'], lw=2.2, label=p['name'])
        ax1.plot(t_ms, gain_env, color=p['color'], lw=2.8, label=p['name'])
        ax1.fill_between(t_ms, 1.0, gain_env, color=p['color'], alpha=0.04)
    
    # Textures/Background
    ax1.plot(t_ms, 0.4 * x + 0.8, color='gray', alpha=0.08, lw=0.5, label='Signal Input (ref)')
    
    ax0.set_ylabel('Bias Shift $\\Delta b(t)$')
    ax0.set_title('(a) Dynamic Bias Accumulation Stress Test (Gallop Rhythm)')
    ax0.legend(loc='upper right', ncol=2, fontsize=8)

    ax1.set_ylim(0.3, 1.1)
    ax1.set_ylabel('Effective Gain (Sag)')
    ax1.set_xlabel('Time [ms]')
    ax1.set_title('(b) Resulting Dynamic Gain Response (The "Musical Feel")')
    ax1.annotate('Fast recovery\nstays "Tight"', xy=(150, 0.95), xytext=(50, 0.45),
                arrowprops=dict(arrowstyle='->', color=COLORS[3]), color=COLORS[3], fontweight='bold')
    ax1.annotate('Complex Breathing\n(Spongy)', xy=(1100, 0.6), xytext=(1150, 0.45),
                arrowprops=dict(arrowstyle='->', color=COLORS[2]), color=COLORS[2], fontweight='bold', ha='center')
    ax1.legend(loc='lower center', ncol=3, fontsize=8)

    fig.suptitle('Cathode Bias — Amplifier Preset Comparison (Rhythmic Stress Test)', fontweight='bold', y=1.01)
    fig.tight_layout()
    path = os.path.join(OUT_DIR, 'fig_cb5_preset_comparison.png')
    fig.savefig(path, bbox_inches='tight')
    print(f"  Saved {path}")
    plt.close(fig)


# ─── Figure 6: Multi-Stage Accumulation (Cascaded Signal Flow) ───────────────
def fig6_multistage():
    N = int(SR * 1.3)
    t_ms = np.arange(N) / SR * 1000
    f = 115.0
    x_in = np.zeros(N)
    pulses = [(0.0, 0.15), (0.2, 0.35), (0.4, 0.55), (0.6, 1.2)]
    for start, end in pulses:
        n = int((end - start) * SR)
        s = int(start * SR)
        x_in[s:s+n] = 0.8 * np.sin(2*np.pi * f * np.arange(n)/SR)

    stage_params = [
        {'name': 'Stage 1: Input (Fast)', 'a': 1.8, 'b': -0.1,  'tau': 28.0,  'scale': 0.12, 'color': COLORS[0]},
        {'name': 'Stage 2: Gain (Meat)',  'a': 4.5, 'b': -0.15, 'tau': 90.0, 'scale': 0.38, 'color': COLORS[1]},
        {'name': 'Stage 3: Lead (Spongy)', 'a': 7.0, 'b': -0.4,  'tau': 380.0, 'scale': 0.65, 'color': COLORS[2]}
    ]

    fig, axes = plt.subplots(3, 1, figsize=(10, 10), sharex=True)
    curr_x = x_in
    for i, p in enumerate(stage_params):
        ax = axes[i]
        cb = cathode_shift(curr_x, p['tau'], p['scale'])
        curr_y = triode_stage(curr_x, p['a'], p['b'], cb)
        gain_env = 1.0 - cb
        
        ax.plot(t_ms, 0.3 * curr_y + 0.75, color=p['color'], alpha=0.15, lw=0.6)
        ax.plot(t_ms, gain_env, color=p['color'], lw=2.5, label=f'Gain Sag: {p["name"]}')
        ax.fill_between(t_ms, 1.0, gain_env, color=p['color'], alpha=0.08)
        
        ax.set_ylim(0.3, 1.1)
        ax.set_ylabel('Effective Gain')
        ax.legend(loc='lower left', fontsize=8)
        
        if i == 0:
            ax.annotate('Preserves Attack', xy=(250, 0.95), xytext=(400, 1.05),
                        arrowprops=dict(arrowstyle='->', color='black'))
        elif i == 1:
            ax.annotate('Progressive Compression', xy=(1100, 0.82), xytext=(1230, 1.05),
                        arrowprops=dict(arrowstyle='->', color='black'), ha='center')
        elif i == 2:
            ax.annotate('Cascading Lag (Spongy)', xy=(1150, 0.65), xytext=(1150, 1.05),
                        arrowprops=dict(arrowstyle='->', color='black'), ha='right')

        curr_x = curr_y * 0.45
        
    axes[2].set_xlabel('Time [ms]')
    fig.suptitle('Cascaded Cathode Bias Dynamics — Preamp Chain Propagation', fontweight='bold', y=1.01)
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
