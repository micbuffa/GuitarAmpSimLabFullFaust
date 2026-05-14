#!/usr/bin/env python3
"""
Generate a publication-quality block diagram for the Guitarix Preamp module.
Shows the wavetable-based tube model architecture with per-stage tube selection.
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
C_OPTION   = '#d4ac0d'
C_ARROW    = '#ecf0f1'
C_TEXT     = '#ecf0f1'
C_LABEL    = '#aeb6bf'
C_ACCENT   = '#e74c3c'
C_TUBE     = '#533483'
C_METER    = '#27ae60'
C_STAGE_BG = '#0f3460'
C_TABLE    = '#b7950b'
C_SELECT   = '#e67e22'


def box(ax, x, y, w, h, lines, color, fontsize=9,
        edgecolor='#4a4a6a', lw=1.2, alpha=0.92):
    """Draw a rounded box with multi-line centered text."""
    patch = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.06",
                           facecolor=color, edgecolor=edgecolor,
                           linewidth=lw, alpha=alpha, zorder=2)
    ax.add_patch(patch)
    n = len(lines)
    usable = h * 0.75
    line_h = min(0.34, usable / max(n, 1))
    top = y + h/2 + (n - 1) * line_h / 2
    for i, (txt, fs, fw, clr) in enumerate(lines):
        ax.text(x + w/2, top - i * line_h, txt,
                ha='center', va='center', fontsize=fs,
                fontweight=fw, color=clr, zorder=3)


def arr(ax, x1, y1, x2, y2, color=C_ARROW, lw=1.5):
    ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle='->', color=color, lw=lw), zorder=4)


def dashed(ax, x, y, w, h, label, color, fontsize=10):
    patch = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.05",
                           facecolor='none', edgecolor=color,
                           linewidth=1.2, linestyle='--', alpha=0.7, zorder=1)
    ax.add_patch(patch)
    ax.text(x + w/2, y + h + 0.08, label, ha='center', va='bottom',
            fontsize=fontsize, color=color, fontweight='bold', zorder=3)


def draw_guitarix_preamp():
    fig, ax = plt.subplots(figsize=(22, 16))
    fig.patch.set_facecolor(C_BG)
    ax.set_facecolor(C_BG)
    ax.set_xlim(-1.0, 21.0)
    ax.set_ylim(-3.0, 13.0)
    ax.axis('off')

    ax.text(10.0, 12.5, 'Guitarix Preamp — Block Diagram',
            ha='center', fontsize=20, fontweight='bold', color=C_TEXT)
    ax.text(10.0, 12.0, 'Wavetable-based tube simulation (ported from Guitarix C++ valve.h)',
            ha='center', fontsize=11, color=C_LABEL, style='italic')

    # ═══════════════════════════════════════════════════════════════════════
    # TOP ROW: Controls
    # ═══════════════════════════════════════════════════════════════════════
    ctrls = [
        (0.0, 10.5, 2.6, 1.0, [
            ('Input Volume', 11, 'bold', C_TEXT),
            ('−20 … +20 dB', 9, 'normal', C_LABEL),
        ], C_GAIN),
        (3.0, 10.5, 2.6, 1.0, [
            ('Interstage Gain', 11, 'bold', C_TEXT),
            ('−20 … +20 dB', 9, 'normal', C_LABEL),
        ], C_GAIN),
        (6.0, 10.5, 2.6, 1.0, [
            ('Output Gain', 11, 'bold', C_TEXT),
            ('−20 … +20 dB', 9, 'normal', C_LABEL),
        ], C_GAIN),
        (9.0, 10.5, 2.6, 1.0, [
            ('Nb Stages', 11, 'bold', C_TEXT),
            ('1 / 2 / 3', 9, 'normal', C_LABEL),
        ], C_OPTION),
        (12.0, 10.5, 2.6, 1.0, [
            ('Bypass', 11, 'bold', C_TEXT),
            ('bypass_fade', 9, 'normal', C_LABEL),
        ], C_BYPASS),
    ]
    for c in ctrls:
        box(ax, *c)

    # ═══════════════════════════════════════════════════════════════════════
    # STAGE 1 (always active)
    # ═══════════════════════════════════════════════════════════════════════
    s1_y = 7.0
    dashed(ax, 0.3, s1_y - 0.3, 19.2, 3.0, 'STAGE 1 (always active)', C_ACCENT, fontsize=12)

    # Input volume
    box(ax, 0.8, s1_y + 1.2, 1.8, 1.0, [
        ('× Input Vol', 10, 'bold', C_TEXT),
        ('ba.db2linear', 8, 'normal', C_LABEL),
    ], C_GAIN)

    # VU meter 1
    box(ax, 3.0, s1_y + 1.2, 1.3, 1.0, [
        ('VU Input', 10, 'bold', '#2ecc71'),
        ('peak-hold', 8, 'normal', C_LABEL),
        ('0.5s decay', 8, 'normal', C_LABEL),
    ], '#16213e', edgecolor='#2ecc71')

    # Tube menu 1
    box(ax, 4.8, s1_y + 1.2, 2.4, 1.0, [
        ('Stage 1 Tube', 10, 'bold', C_TEXT),
        ('Menu: 6 tube types', 8, 'normal', C_LABEL),
    ], C_SELECT)

    # Stage 1 multiplexer
    box(ax, 7.6, s1_y + 0.0, 6.0, 2.5, [
        ('ba.selectn(6)', 12, 'bold', C_TEXT),
    ], C_STAGE_BG, edgecolor='#4a7abf')

    # 6 tube wavetables inside
    tube_names = ['12AX7', '12AT7', '12AU7', '6V6', '6DJ8', '6C16']
    tube_colors = ['#8e44ad', '#2980b9', '#27ae60', '#c0392b', '#d68910', '#1abc9c']
    for i, (name, clr) in enumerate(zip(tube_names, tube_colors)):
        bx = 7.9 + i * 0.92
        box(ax, bx, s1_y + 0.2, 0.78, 1.4, [
            (name, 8, 'bold', C_TEXT),
            ('T1_', 6, 'normal', C_LABEL),
        ], clr, edgecolor='#555', lw=0.8, alpha=0.85)

    # VU meter 2
    box(ax, 14.0, s1_y + 1.2, 1.5, 1.0, [
        ('VU Inter 1-2', 10, 'bold', '#2ecc71'),
        ('peak-hold', 8, 'normal', C_LABEL),
    ], '#16213e', edgecolor='#2ecc71')

    # Arrows for stage 1
    arr(ax, 2.6, s1_y + 1.7, 3.0, s1_y + 1.7)
    arr(ax, 4.3, s1_y + 1.7, 4.8, s1_y + 1.7)
    arr(ax, 7.2, s1_y + 1.7, 7.6, s1_y + 1.7)
    arr(ax, 13.6, s1_y + 1.7, 14.0, s1_y + 1.7)

    # Arrow from tube menu to selectn
    ax.annotate('', xy=(8.5, s1_y + 2.5), xytext=(7.0, s1_y + 1.7),
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=1.2,
                                connectionstyle='arc3,rad=-0.2'), zorder=4)

    # Input arrow
    ax.annotate('Guitar\nInput', xy=(0.8, s1_y + 1.7), xytext=(-0.5, s1_y + 1.7),
                fontsize=11, color=C_OPTION, fontweight='bold', ha='center', va='center',
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=2))

    # ═══════════════════════════════════════════════════════════════════════
    # STAGE 2 (if nstages > 0)
    # ═══════════════════════════════════════════════════════════════════════
    s2_y = 3.3
    dashed(ax, 0.3, s2_y - 0.3, 19.2, 3.0, 'STAGE 2 (if nstages > 0)', '#2ecc71', fontsize=12)

    # Interstage gain
    box(ax, 0.8, s2_y + 1.2, 1.8, 1.0, [
        ('× Interstage', 10, 'bold', C_TEXT),
        ('ba.db2linear', 8, 'normal', C_LABEL),
    ], C_GAIN)

    # LP filter
    box(ax, 3.0, s2_y + 1.2, 2.0, 1.0, [
        ('LPF 6531 Hz', 10, 'bold', C_TEXT),
        ('fi.lowpass(1)', 8, 'normal', C_LABEL),
    ], C_FILTER)

    # Tube menu 2
    box(ax, 5.4, s2_y + 1.2, 2.0, 1.0, [
        ('Stage 2 Tube', 10, 'bold', C_TEXT),
        ('Menu: 6 types', 8, 'normal', C_LABEL),
    ], C_SELECT)

    # Stage 2 multiplexer
    box(ax, 7.8, s2_y + 0.0, 5.6, 2.5, [
        ('ba.selectn(6)', 12, 'bold', C_TEXT),
    ], C_STAGE_BG, edgecolor='#4a7abf')

    for i, (name, clr) in enumerate(zip(tube_names, tube_colors)):
        bx = 8.05 + i * 0.88
        box(ax, bx, s2_y + 0.2, 0.74, 1.4, [
            (name, 8, 'bold', C_TEXT),
            ('T2_', 6, 'normal', C_LABEL),
        ], clr, edgecolor='#555', lw=0.8, alpha=0.85)

    # VU meter 3
    box(ax, 13.8, s2_y + 1.2, 1.5, 1.0, [
        ('VU Output', 10, 'bold', '#2ecc71'),
        ('peak-hold', 8, 'normal', C_LABEL),
    ], '#16213e', edgecolor='#2ecc71')

    # Arrows for stage 2
    arr(ax, 2.6, s2_y + 1.7, 3.0, s2_y + 1.7)
    arr(ax, 5.0, s2_y + 1.7, 5.4, s2_y + 1.7)
    arr(ax, 7.4, s2_y + 1.7, 7.8, s2_y + 1.7)
    arr(ax, 13.4, s2_y + 1.7, 13.8, s2_y + 1.7)

    # Connect stage 1 output → stage 2 input
    ax.annotate('', xy=(0.8, s2_y + 1.7), xytext=(15.5, s1_y + 1.2),
                arrowprops=dict(arrowstyle='->', color=C_ARROW, lw=1.5,
                                connectionstyle='arc3,rad=0.4'), zorder=4)

    ax.annotate('', xy=(8.5, s2_y + 2.5), xytext=(7.2, s2_y + 1.7),
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=1.2,
                                connectionstyle='arc3,rad=-0.2'), zorder=4)

    # select2 label
    ax.text(16.0, s2_y + 2.3, 'select2\n(nstages > 0)',
            fontsize=9, color=C_ACCENT, ha='center', va='center',
            bbox=dict(boxstyle='round,pad=0.15', facecolor=C_BG,
                      edgecolor=C_ACCENT, linewidth=0.8))

    # ═══════════════════════════════════════════════════════════════════════
    # STAGE 3 (if nstages > 1)
    # ═══════════════════════════════════════════════════════════════════════
    s3_y = -0.4
    dashed(ax, 0.3, s3_y - 0.3, 19.2, 3.0, 'STAGE 3 (if nstages > 1)', '#3498db', fontsize=12)

    # Interstage gain
    box(ax, 0.8, s3_y + 1.2, 1.8, 1.0, [
        ('× Interstage', 10, 'bold', C_TEXT),
        ('ba.db2linear', 8, 'normal', C_LABEL),
    ], C_GAIN)

    # LP filter
    box(ax, 3.0, s3_y + 1.2, 2.0, 1.0, [
        ('LPF 6531 Hz', 10, 'bold', C_TEXT),
        ('fi.lowpass(1)', 8, 'normal', C_LABEL),
    ], C_FILTER)

    # Tube menu 3
    box(ax, 5.4, s3_y + 1.2, 2.0, 1.0, [
        ('Stage 3 Tube', 10, 'bold', C_TEXT),
        ('Menu: 6 types', 8, 'normal', C_LABEL),
    ], C_SELECT)

    # Stage 3 multiplexer
    box(ax, 7.8, s3_y + 0.0, 5.6, 2.5, [
        ('ba.selectn(6)', 12, 'bold', C_TEXT),
    ], C_STAGE_BG, edgecolor='#4a7abf')

    for i, (name, clr) in enumerate(zip(tube_names, tube_colors)):
        bx = 8.05 + i * 0.88
        box(ax, bx, s3_y + 0.2, 0.74, 1.4, [
            (name, 8, 'bold', C_TEXT),
            ('T3_', 6, 'normal', C_LABEL),
        ], clr, edgecolor='#555', lw=0.8, alpha=0.85)

    # Arrows for stage 3
    arr(ax, 2.6, s3_y + 1.7, 3.0, s3_y + 1.7)
    arr(ax, 5.0, s3_y + 1.7, 5.4, s3_y + 1.7)
    arr(ax, 7.4, s3_y + 1.7, 7.8, s3_y + 1.7)

    # Connect stage 2 output → stage 3 input
    ax.annotate('', xy=(0.8, s3_y + 1.7), xytext=(15.3, s2_y + 1.2),
                arrowprops=dict(arrowstyle='->', color=C_ARROW, lw=1.5,
                                connectionstyle='arc3,rad=0.4'), zorder=4)

    ax.annotate('', xy=(8.5, s3_y + 2.5), xytext=(7.2, s3_y + 1.7),
                arrowprops=dict(arrowstyle='->', color=C_OPTION, lw=1.2,
                                connectionstyle='arc3,rad=-0.2'), zorder=4)

    ax.text(16.0, s3_y + 2.3, 'select2\n(nstages > 1)',
            fontsize=9, color='#3498db', ha='center', va='center',
            bbox=dict(boxstyle='round,pad=0.15', facecolor=C_BG,
                      edgecolor='#3498db', linewidth=0.8))

    # ═══════════════════════════════════════════════════════════════════════
    # OUTPUT SECTION
    # ═══════════════════════════════════════════════════════════════════════
    out_y = s3_y + 0.5

    # Output gain + gk_comp
    box(ax, 14.5, out_y, 2.5, 1.5, [
        ('× Output Gain', 11, 'bold', C_TEXT),
        ('× gk_comp', 9, 'normal', C_LABEL),
        ('dynamic comp.', 8, 'normal', '#e74c3c'),
    ], C_GAIN)

    arr(ax, 13.4, s3_y + 1.7, 14.5, out_y + 0.75)

    # Bypass
    box(ax, 17.5, out_y, 2.0, 1.5, [
        ('bypass_fade', 10, 'bold', C_TEXT),
        ('SR/10 fade', 8, 'normal', C_LABEL),
    ], C_BYPASS)
    arr(ax, 17.0, out_y + 0.75, 17.5, out_y + 0.75)

    # Output
    arr(ax, 19.5, out_y + 0.75, 20.0, out_y + 0.75, color=C_OPTION, lw=2)
    ax.text(20.2, out_y + 0.75, 'To\nCoupling EQ',
            fontsize=11, color=C_OPTION, fontweight='bold', ha='left', va='center')

    # ═══════════════════════════════════════════════════════════════════════
    # DETAIL: Wavetable structure (bottom right)
    # ═══════════════════════════════════════════════════════════════════════
    detail_y = -2.5
    dashed(ax, 0.3, detail_y - 0.3, 12.5, 2.2,
           'Tube Model Detail — rdtable interpolated lookup', C_TABLE, fontsize=11)

    box(ax, 0.6, detail_y, 2.2, 1.5, [
        ('Vgk Input', 10, 'bold', C_TEXT),
        ('grid-cathode V', 8, 'normal', C_LABEL),
    ], C_FILTER)

    box(ax, 3.2, detail_y, 2.5, 1.5, [
        ('Index Compute', 10, 'bold', C_TEXT),
        ('f = (Vgk−low)×istep', 8, 'normal', C_LABEL),
        ('bounds check', 8, 'normal', C_LABEL),
    ], C_TABLE)

    box(ax, 6.1, detail_y, 2.5, 1.5, [
        ('rdtable ×2', 10, 'bold', C_TEXT),
        ('val0 = wf[i]', 8, 'normal', C_LABEL),
        ('val1 = wf[i+1]', 8, 'normal', C_LABEL),
    ], C_TUBE)

    box(ax, 9.0, detail_y, 2.8, 1.5, [
        ('Linear Interp.', 10, 'bold', C_TEXT),
        ('v0·(1−f) + v1·f', 8, 'normal', C_LABEL),
        ('→ Plate voltage', 8, 'normal', C_LABEL),
    ], C_STAGE_BG)

    arr(ax, 2.8, detail_y + 0.75, 3.2, detail_y + 0.75)
    arr(ax, 5.7, detail_y + 0.75, 6.1, detail_y + 0.75)
    arr(ax, 8.6, detail_y + 0.75, 9.0, detail_y + 0.75)

    # Wavetable spec
    ax.text(6.5, detail_y - 0.15,
            'Wavetable: 2001 samples  ·  range [−5, +5] V  ·  step = 200 samples/V  ·  68k / 250k load variants',
            fontsize=8, color=C_LABEL, ha='center', style='italic')

    # ═══════════════════════════════════════════════════════════════════════
    # DETAIL: gk_comp (bottom right)
    # ═══════════════════════════════════════════════════════════════════════
    dashed(ax, 13.5, detail_y - 0.3, 6.5, 2.2,
           'Dynamic Gain Compensation (gk_comp)', '#e74c3c', fontsize=10)

    box(ax, 13.8, detail_y, 2.8, 1.5, [
        ('ref / preamp', 10, 'bold', C_TEXT),
        ('ref = dB2lin(−6)', 8, 'normal', C_LABEL),
        ('inverse ratio', 8, 'normal', C_LABEL),
    ], '#922b21')

    box(ax, 17.0, detail_y, 2.5, 1.5, [
        ('select2', 10, 'bold', C_TEXT),
        ('1-stage → 1.0', 8, 'normal', C_LABEL),
        ('else → ratio', 8, 'normal', C_LABEL),
    ], '#922b21')

    arr(ax, 16.6, detail_y + 0.75, 17.0, detail_y + 0.75)

    plt.tight_layout(pad=0.3)
    return fig


if __name__ == '__main__':
    out_dir = os.path.dirname(os.path.abspath(__file__))
    fig = draw_guitarix_preamp()
    path = os.path.join(out_dir, 'guitarix_preamp_architecture.png')
    fig.savefig(path, dpi=200, facecolor=fig.get_facecolor())
    print(f'Saved: {path}')
    plt.close(fig)
