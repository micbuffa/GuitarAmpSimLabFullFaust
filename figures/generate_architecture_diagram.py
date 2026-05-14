#!/usr/bin/env python3
"""
Generate publication-quality architecture diagrams of the Faust guitar amp simulator.
Two figures:
  1. Top-level signal chain
  2. Detailed preamp internal architecture
"""

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch
import os

# ── Colour palette ───────────────────────────────────────────────────────────
C_BG       = '#1a1a2e'
C_FILTER   = '#1a5276'
C_GAIN     = '#1e8449'
C_BYPASS   = '#7d3c98'
C_DYN      = '#c0392b'
C_OPTION   = '#d4ac0d'
C_ARROW    = '#ecf0f1'
C_TEXT     = '#ecf0f1'
C_LABEL    = '#aeb6bf'
C_ACCENT   = '#e74c3c'
C_PREAMP   = '#0f3460'
C_TONESTACK= '#2e86c1'
C_POWERAMP = '#a04000'
C_CABSIM   = '#1abc9c'
C_REVERB   = '#8e44ad'
C_TUBE     = '#533483'
C_GC       = '#b7950b'
C_CATH     = '#1e8449'

def box(ax, x, y, w, h, lines, color, fontsize=9,
        edgecolor='#4a4a6a', lw=1.2, alpha=0.92):
    """Draw a rounded box with multi-line centered text."""
    patch = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                           facecolor=color, edgecolor=edgecolor,
                           linewidth=lw, alpha=alpha, zorder=2)
    ax.add_patch(patch)
    n = len(lines)
    # Use 75% of box height for text, spread evenly
    usable = h * 0.75
    line_h = min(0.16, usable / max(n, 1))
    top = y + h/2 + (n - 1) * line_h / 2
    for i, (txt, fs, fw, clr) in enumerate(lines):
        ax.text(x + w/2, top - i * line_h, txt,
                ha='center', va='center', fontsize=fs,
                fontweight=fw, color=clr, zorder=3)

def arr(ax, x1, y1, x2, y2, color=C_ARROW, lw=1.5):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='->', color=color, lw=lw), zorder=4)

def dashed(ax, x, y, w, h, label, color, fontsize=9):
    patch = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.04",
                           facecolor='none', edgecolor=color,
                           linewidth=1.0, linestyle='--', alpha=0.7, zorder=1)
    ax.add_patch(patch)
    ax.text(x + w/2, y + h + 0.04, label, ha='center', va='bottom',
            fontsize=fontsize, color=color, fontweight='bold', zorder=3)

# =============================================================================
# FIGURE 1 — Top-level signal chain
# =============================================================================
def draw_top_level():
    fig, ax = plt.subplots(figsize=(20, 4.5))
    fig.patch.set_facecolor(C_BG); ax.set_facecolor(C_BG)
    ax.set_xlim(-0.8, 15.0); ax.set_ylim(-0.3, 2.0); ax.axis('off')

    ax.text(7.5, 1.8, '100% Faust Guitar Tube Amp Simulator — Signal Chain',
            ha='center', fontsize=16, fontweight='bold', color=C_TEXT)

    # (x, y, w, h, [lines], color)
    bw, bh = 1.15, 0.9
    stages = [
        (0.0,  0.3, bw, bh, [
            ('DC Block + HPF', 9, 'bold', C_TEXT),
            ('fi.dcblocker', 7, 'normal', C_LABEL),
            ('23 Hz highpass', 7, 'normal', C_LABEL),
        ], C_FILTER),
        (1.4, 0.3, bw, bh, [
            ('Input Filter', 10, 'bold', C_TEXT),
            ('+2.4 dB @ 1.2 kHz', 7, 'normal', C_LABEL),
            ('pick attack boost', 7, 'normal', C_LABEL),
        ], C_FILTER),
        (2.8, 0.1, 1.5, 1.3, [
            ('PREAMP', 13, 'bold', C_TEXT),
            ('1–3 triode stages', 8, 'normal', C_LABEL),
            ('9 amp presets', 8, 'normal', C_LABEL),
            ('Static / Dynamic', 8, 'normal', C_LABEL),
        ], C_PREAMP),
        (4.55, 0.3, 1.3, bh, [
            ('Coupling EQ', 10, 'bold', C_TEXT),
            ('+8.5 dB bass', 7, 'normal', C_LABEL),
            ('+13.6 dB presence', 7, 'normal', C_LABEL),
            ('−10 dB treble', 7, 'normal', C_LABEL),
        ], C_FILTER),
        (6.1, 0.3, bw, bh, [
            ('Tonestack', 10, 'bold', C_TEXT),
            ('4 models', 7, 'normal', C_LABEL),
            ('Bass / Mid / Treble', 7, 'normal', C_LABEL),
            ('Bypass', 7, 'normal', '#e74c3c'),
        ], C_TONESTACK),
        (7.5, 0.3, bw, bh, [
            ('Power Amp', 10, 'bold', C_TEXT),
            ('Master Volume', 7, 'normal', C_LABEL),
            ('Waveshaper loop', 7, 'normal', C_LABEL),
            ('Bypass', 7, 'normal', '#e74c3c'),
        ], C_POWERAMP),
        (8.9, 0.3, 0.9, bh, [
            ('DC Block', 10, 'bold', C_TEXT),
            ('fi.dcblocker', 7, 'normal', C_LABEL),
        ], C_FILTER),
        (10.0, 0.3, 1.0, bh, [
            ('Output Level', 10, 'bold', C_TEXT),
            ('−42 … −18 dB', 7, 'normal', C_LABEL),
        ], C_GAIN),
        (11.2, 0.3, bw, bh, [
            ('Reverb', 10, 'bold', C_TEXT),
            ('Multi-algorithm', 7, 'normal', C_LABEL),
            ('Bypass', 7, 'normal', '#e74c3c'),
        ], C_REVERB),
        (12.6, 0.3, bw, bh, [
            ('Cabinet Sim', 10, 'bold', C_TEXT),
            ('3 FIR impulses', 7, 'normal', C_LABEL),
            ('Makeup gain / IR', 7, 'normal', C_LABEL),
            ('Bypass', 7, 'normal', '#e74c3c'),
        ], C_CABSIM),
    ]

    for s in stages:
        box(ax, *s)

    # Arrows
    pairs = [(1.15, 0.75, 1.4, 0.75), (2.55, 0.75, 2.8, 0.75),
             (4.3, 0.75, 4.55, 0.75), (5.85, 0.75, 6.1, 0.75),
             (7.25, 0.75, 7.5, 0.75), (8.65, 0.75, 8.9, 0.75),
             (9.8, 0.75, 10.0, 0.75), (11.0, 0.75, 11.2, 0.75),
             (12.35, 0.75, 12.6, 0.75)]
    for a in pairs:
        arr(ax, *a)

    # Input/Output labels
    ax.annotate('Guitar\nInput', xy=(0.0, 0.75), xytext=(-0.65, 0.75),
                fontsize=10, color=C_OPTION, fontweight='bold', ha='center', va='center',
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=2))
    arr(ax, 13.75, 0.75, 14.15, 0.75, color=C_OPTION, lw=2)
    ax.text(14.25, 0.75, 'Stereo\nOutput', fontsize=10, color=C_OPTION,
            fontweight='bold', ha='left', va='center')

    # Preamp annotation
    ax.annotate('See detailed diagram below', xy=(3.55, 0.1), xytext=(3.55, -0.15),
                fontsize=8, color=C_ACCENT, ha='center', va='top',
                arrowprops=dict(arrowstyle='->', color=C_ACCENT, lw=1))

    ax.text(13.2, 0.1, 'mono→stereo', fontsize=7, color=C_LABEL, ha='center', style='italic')

    plt.tight_layout(pad=0.3)
    return fig


# =============================================================================
# FIGURE 2 — Detailed preamp
# =============================================================================
def draw_preamp():
    fig, ax = plt.subplots(figsize=(22, 13))
    fig.patch.set_facecolor(C_BG); ax.set_facecolor(C_BG)
    ax.set_xlim(-0.5, 16.5); ax.set_ylim(-2.5, 9.5); ax.axis('off')

    ax.text(8.0, 9.2, 'Preamp Architecture — Detailed Block Diagram',
            ha='center', fontsize=17, fontweight='bold', color=C_TEXT)

    # ── Top controls row ──
    ctrls = [
        (0.0, 7.8, 1.8, 0.85, [
            ('Amp Model', 10, 'bold', C_TEXT),
            ('9 presets', 8, 'normal', C_LABEL),
            ('(Custom + 8 amps)', 7, 'normal', C_LABEL),
        ], C_OPTION),
        (2.1, 7.8, 1.3, 0.85, [
            ('Input Vol', 10, 'bold', C_TEXT),
            ('0 – 3', 8, 'normal', C_LABEL),
        ], C_GAIN),
        (3.7, 7.8, 1.5, 0.85, [
            ('Interstage Gain', 10, 'bold', C_TEXT),
            ('0.01 – 1.0', 8, 'normal', C_LABEL),
            ('× gk_scale / preset', 7, 'normal', C_LABEL),
        ], C_GAIN),
        (5.5, 7.8, 1.3, 0.85, [
            ('Master Vol', 10, 'bold', C_TEXT),
            ('0 – 5', 8, 'normal', C_LABEL),
        ], C_GAIN),
        (7.1, 7.8, 2.0, 0.85, [
            ('Stage Model', 10, 'bold', C_TEXT),
            ('Static / Dyn Match', 8, 'normal', C_LABEL),
            ('/ Dyn Manual', 8, 'normal', C_LABEL),
        ], C_DYN),
        (9.4, 7.8, 1.6, 0.85, [
            ('ADAA Oversamp.', 10, 'bold', C_TEXT),
            ('aa.tanh1', 8, 'normal', C_LABEL),
        ], C_BYPASS),
        (11.3, 7.8, 1.2, 0.85, [
            ('Stages', 10, 'bold', C_TEXT),
            ('1 / 2 / 3', 8, 'normal', C_LABEL),
        ], C_OPTION),
        (12.8, 7.8, 1.2, 0.85, [
            ('Bypass', 10, 'bold', C_TEXT),
            ('Dry path', 8, 'normal', C_LABEL),
        ], C_BYPASS),
    ]
    for c in ctrls:
        box(ax, *c)

    # Presets legend
    ax.text(8.0, 7.4, 'Presets: Custom · Marshall JCM800 · Fender Twin · Mesa Dual Rect · '
            'Vox AC30 · Peavey 5150 · Fender Deluxe · Soldano SLO-100 · Orange Rockerverb',
            ha='center', fontsize=8, color=C_LABEL, style='italic')

    # ── Helper to draw one stage ──
    def draw_stage(sy, label, label_color, stage_n):
        dashed(ax, 0.1, sy - 0.1, 5.0, 2.3, label, label_color, fontsize=10)

        # Grid Current
        box(ax, 0.3, sy + 1.1, 1.2, 0.8, [
            ('Grid Current', 9, 'bold', C_TEXT),
            ('vth, amtIn', 7, 'normal', C_LABEL),
            ('amtBias, τ', 7, 'normal', C_LABEL),
        ], C_GC)

        # Static Triode
        box(ax, 1.8, sy + 1.1, 1.4, 0.8, [
            ('Triode (Static)', 9, 'bold', C_TEXT),
            (f'tanh(a{stage_n}·x + b{stage_n})', 7, 'normal', C_LABEL),
            ('or aa.tanh1', 7, 'normal', C_LABEL),
        ], C_TUBE)

        # Cathode Bias
        box(ax, 3.5, sy + 1.1, 1.3, 0.8, [
            ('Cathode Bias', 9, 'bold', C_TEXT),
            ('RC sag', 7, 'normal', C_LABEL),
            ('τ, scale', 7, 'normal', C_LABEL),
        ], C_CATH)

        # Dynamic alternative
        box(ax, 1.8, sy + 0.05, 1.4, 0.7, [
            ('Dynamic Tube', 9, 'bold', C_TEXT),
            ('(MODEL 2)', 7, 'normal', C_LABEL),
            ('if selected', 7, 'normal', '#e74c3c'),
        ], C_DYN)

        # Switch symbol
        ax.text(3.35, sy + 0.4, '⇌', fontsize=16, color=C_ACCENT,
                ha='center', va='center', fontweight='bold', zorder=5)

        # Internal arrows
        arr(ax, 1.5, sy + 1.5, 1.8, sy + 1.5)
        arr(ax, 3.2, sy + 1.5, 3.5, sy + 1.5)

        # VU meter badge
        ax.text(5.0, sy + 2.0, f'VU{stage_n}', fontsize=7, color='#2ecc71',
                ha='center', va='center', fontweight='bold',
                bbox=dict(boxstyle='round,pad=0.12', facecolor=C_BG,
                          edgecolor='#2ecc71', linewidth=0.8), zorder=5)

    # ── Draw 3 stages ──
    draw_stage(5.0, 'STAGE 1 (always active)', C_ACCENT, 1)
    draw_stage(2.6, 'STAGE 2 (if stages ≥ 2)', '#2ecc71', 2)
    draw_stage(0.2, 'STAGE 3 (if stages = 3)', '#3498db', 3)

    # ── Interstage blocks ──
    box(ax, 5.6, 5.8, 1.3, 0.8, [
        ('× gk', 11, 'bold', C_TEXT),
        ('gain_knob', 7, 'normal', C_LABEL),
        ('× gk_scale', 7, 'normal', C_LABEL),
    ], C_GAIN)
    arr(ax, 5.1, 6.2, 5.6, 6.2)

    box(ax, 7.2, 5.8, 1.8, 0.8, [
        ('Interstage Filter 1→2', 9, 'bold', C_TEXT),
        ('HPF + LPF', 7, 'normal', C_LABEL),
        ('per preset', 7, 'normal', C_LABEL),
    ], C_FILTER)
    arr(ax, 6.9, 6.2, 7.2, 6.2)

    # Arrow down from filter 1→2 to Stage 2
    ax.annotate('', xy=(2.5, 4.9), xytext=(8.2, 5.8),
                arrowprops=dict(arrowstyle='->', color=C_ARROW, lw=1.5,
                                connectionstyle='arc3,rad=0.35'), zorder=4)

    box(ax, 5.6, 3.4, 1.8, 0.8, [
        ('Interstage Filter 2→3', 9, 'bold', C_TEXT),
        ('HPF + LPF', 7, 'normal', C_LABEL),
        ('per preset', 7, 'normal', C_LABEL),
    ], C_FILTER)
    arr(ax, 5.1, 3.8, 5.6, 3.8)

    # Arrow down from filter 2→3 to Stage 3
    ax.annotate('', xy=(2.5, 2.5), xytext=(6.5, 3.4),
                arrowprops=dict(arrowstyle='->', color=C_ARROW, lw=1.5,
                                connectionstyle='arc3,rad=0.35'), zorder=4)

    # ── Output block ──
    box(ax, 5.6, 0.8, 1.6, 0.9, [
        ('Output Gain', 10, 'bold', C_TEXT),
        ('× preamp_out', 7, 'normal', C_LABEL),
        ('× level_comp', 7, 'normal', C_LABEL),
        ('× gk_comp', 7, 'normal', C_LABEL),
    ], C_GAIN)
    arr(ax, 5.1, 1.25, 5.6, 1.25)

    box(ax, 7.6, 0.8, 1.8, 0.9, [
        ('Post-Preamp EQ', 10, 'bold', C_TEXT),
        ('Hi-Cut (optional)', 7, 'normal', C_LABEL),
        ('Lo-Cut (optional)', 7, 'normal', C_LABEL),
    ], C_FILTER)
    arr(ax, 7.2, 1.25, 7.6, 1.25)

    # VU out
    ax.text(7.4, 1.9, 'VU4', fontsize=7, color='#2ecc71',
            ha='center', va='center', fontweight='bold',
            bbox=dict(boxstyle='round,pad=0.12', facecolor=C_BG,
                      edgecolor='#2ecc71', linewidth=0.8), zorder=5)

    # Output arrow
    arr(ax, 9.4, 1.25, 9.8, 1.25, color=C_OPTION, lw=2)
    ax.text(10.0, 1.25, 'To Coupling EQ\n→ Tonestack\n→ Power Amp',
            fontsize=9, color=C_OPTION, fontweight='bold', ha='left', va='center')

    # Input arrow
    ax.annotate('From\nInput Filter', xy=(0.3, 6.5), xytext=(-0.7, 6.5),
                fontsize=9, color=C_OPTION, fontweight='bold', ha='center', va='center',
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=2))

    # ── DETAIL: Dynamic Tube Stage ──
    dashed(ax, 9.5, -2.2, 6.5, 3.2, 'Dynamic Tube Stage (MODEL 2) — Internal Detail', C_DYN, fontsize=10)

    box(ax, 9.8, -0.4, 1.3, 0.9, [
        ('Envelope', 10, 'bold', C_TEXT),
        ('Followers', 10, 'bold', C_TEXT),
        ('eF (fast τF)', 7, 'normal', C_LABEL),
        ('eS (slow τS)', 7, 'normal', C_LABEL),
    ], C_DYN)

    box(ax, 11.4, -0.4, 1.3, 0.9, [
        ('Drive', 10, 'bold', C_TEXT),
        ('Modulation', 10, 'bold', C_TEXT),
        ('d₀ + kF·eF − kS·eS', 7, 'normal', C_LABEL),
    ], C_DYN)

    box(ax, 13.0, -0.4, 1.3, 0.9, [
        ('Bias', 10, 'bold', C_TEXT),
        ('Modulation', 10, 'bold', C_TEXT),
        ('b₀ + kB·eS', 7, 'normal', C_LABEL),
    ], C_DYN)

    box(ax, 14.6, -0.4, 1.2, 0.9, [
        ('Asymmetric', 10, 'bold', C_TEXT),
        ('tanh', 10, 'bold', C_TEXT),
        ('tanh(d·x + b)', 7, 'normal', C_LABEL),
    ], C_DYN)

    arr(ax, 11.1, 0.05, 11.4, 0.05)
    arr(ax, 12.7, 0.05, 13.0, 0.05)
    arr(ax, 14.3, 0.05, 14.6, 0.05)

    # Parameters
    params = ['inScale', 'drive₀', 'biasOff.', 'τF / τS', 'kF/kS/kB', 'postGain']
    for i, p in enumerate(params):
        ax.text(9.9 + i * 1.1, -1.5, p, fontsize=7, color=C_LABEL,
                ha='center', va='center',
                bbox=dict(boxstyle='round,pad=0.1', facecolor='#16213e',
                          edgecolor='#555', linewidth=0.5))

    ax.text(12.7, -2.0, '3 modes:  Static (off)  ·  Dynamic (Match Amp Preset)  ·  Dynamic (Manual Knobs)',
            fontsize=8, color=C_LABEL, ha='center', style='italic')

    # ── DETAIL: Grid Current ──
    dashed(ax, 0.1, -2.2, 4.0, 1.7, 'Grid Current Block — Detail', '#f39c12', fontsize=9)

    box(ax, 0.3, -1.7, 1.1, 0.8, [
        ('Threshold', 9, 'bold', C_TEXT),
        ('Detect', 9, 'bold', C_TEXT),
        ('max(0, x−vth)', 7, 'normal', C_LABEL),
    ], C_GC)

    box(ax, 1.7, -1.7, 1.0, 0.8, [
        ('Envelope', 9, 'bold', C_TEXT),
        ('Follower', 9, 'bold', C_TEXT),
        ('RC (τ ms)', 7, 'normal', C_LABEL),
    ], C_GC)

    box(ax, 3.0, -1.7, 0.9, 0.8, [
        ('Bias', 9, 'bold', C_TEXT),
        ('Shift', 9, 'bold', C_TEXT),
        ('amtBias', 7, 'normal', C_LABEL),
    ], C_GC)

    arr(ax, 1.4, -1.3, 1.7, -1.3)
    arr(ax, 2.7, -1.3, 3.0, -1.3)

    # ── DETAIL: Cathode Bias ──
    dashed(ax, 4.5, -2.2, 4.5, 1.7, 'Cathode Bias Block — Detail', '#27ae60', fontsize=9)

    box(ax, 4.7, -1.7, 1.2, 0.8, [
        ('Half-wave', 9, 'bold', C_TEXT),
        ('Rectify', 9, 'bold', C_TEXT),
        ('max(0, x)', 7, 'normal', C_LABEL),
    ], C_CATH)

    box(ax, 6.1, -1.7, 1.1, 0.8, [
        ('RC Smooth', 9, 'bold', C_TEXT),
        ('τ = Rk·Ck', 7, 'normal', C_LABEL),
    ], C_CATH)

    box(ax, 7.5, -1.7, 1.3, 0.8, [
        ('Scale &', 9, 'bold', C_TEXT),
        ('Subtract', 9, 'bold', C_TEXT),
        ('bias shift', 7, 'normal', C_LABEL),
    ], C_CATH)

    arr(ax, 5.9, -1.3, 6.1, -1.3)
    arr(ax, 7.2, -1.3, 7.5, -1.3)

    plt.tight_layout(pad=0.3)
    return fig


if __name__ == '__main__':
    out_dir = os.path.dirname(os.path.abspath(__file__))

    fig1 = draw_top_level()
    p1 = os.path.join(out_dir, 'amp_sim_signal_chain.png')
    fig1.savefig(p1, dpi=200, facecolor=fig1.get_facecolor())
    print(f'Saved: {p1}')
    plt.close(fig1)

    fig2 = draw_preamp()
    p2 = os.path.join(out_dir, 'preamp_architecture.png')
    fig2.savefig(p2, dpi=200, facecolor=fig2.get_facecolor())
    print(f'Saved: {p2}')
    plt.close(fig2)
