import("stdfaust.lib");

// ------------------------------------------------------------
// IFC_dynamicTubeMB_asym.dsp by Michel Buffa
// Generic bloc for asymetric dynamic tube
// This is NOT just a waveshaper
// This is a dynamic tube stage model. Every parameter has a real physical interpretation.
// ------------------------------------------------------------
/*
MODEL : y=tanh(g(e)⋅x+b(e)) where g(e) = drive modulated by enveloppe, and b(e) = bias modulated by enveloppe
is a dynamic nonlinear stage, equivalent to a triode gain stage where:
- gain varies with signal
- bias can shift
- power supply reacts (sag)

High-level mapping:

DSP parameter	Real tube equivalent
------------------------------------
tanh	    tube transfer curve 𝐼𝑎(𝑉𝑔𝑘)
drive	    triode gain (µ, load resistor)
bias	    grid-to-cathode voltage 𝑉𝑔𝑘
envelope	power supply + cathode dynamics
tau	        RC time constants

*/

clamp(lo, hi, x) = max(lo, min(hi, x));

// Envelope follower simple
envFollower(tauMs) = abs : si.smooth(alpha)
with {
    alpha = clamp(0.0001, 0.9999, 1.0 / max(1.0, (tauMs * ma.SR * 0.001)));
};

// ADAA Oversampling feature
os_enable_dyn = hgroup("2 Preamp", checkbox("Oversampling MODEL 2[stratus:3]")) : int;

logcosh(x) = abs(x) - 0.69314718056 + log(1.0 + exp(-2.0 * abs(x)));

// Saturation asymétrique simple avec anti-aliasing (ADAA1)
F_biased(drive, bias, x) = logcosh(drive * (x + bias)) / max(0.0001, drive);

adaa1_biased(drive, bias, x) = y
with {
    x1 = x';
    diff = x - x1;
    safe_diff = ba.if(abs(diff) < 0.05, 1.0, diff);
    fallback = ma.tanh(drive * (x + bias));
    y = ba.if((abs(drive * (x + bias)) > 5.0) | (abs(diff) < 0.05), fallback, (F_biased(drive, bias, x) - F_biased(drive, bias, x1)) / safe_diff);
};

asymBiasedTanh(drive, bias, x) = select2(os_enable_dyn, ma.tanh(drive * (x + bias)), adaa1_biased(drive, bias, x));

// Variante à pentes différentes avec ADAA1
F_asym(dP, dN, x) = ba.if(x >= 0.0,
    logcosh(dP * x) / max(0.0001, dP),
    logcosh(dN * x) / max(0.0001, dN)
);

adaa1_asym(dP, dN, x) = y
with {
    x1 = x';
    diff = x - x1;
    safe_diff = ba.if(abs(diff) < 0.05, 1.0, diff);
    fallback = (x >= 0.0) * ma.tanh(dP * x) + (x < 0.0) * ma.tanh(dN * x);
    arg = ba.if(x >= 0.0, dP * x, dN * x);
    y = ba.if((abs(arg) > 5.0) | (abs(diff) < 0.05), fallback, (F_asym(dP, dN, x) - F_asym(dP, dN, x1)) / safe_diff);
};

asymDualTanh(drivePos, driveNeg, x) = select2(os_enable_dyn, 
    ((x >= 0.0) * ma.tanh(drivePos * x) + (x < 0.0) * ma.tanh(driveNeg * x)), 
    adaa1_asym(drivePos, driveNeg, x)
);

// Bloc principal
/*
- inGain = Input signal level into the stage. Corresponds to previous stage output, coupling capacitor, gain pot. "how hard you hit the tube"
- drive0 corresponds to the intrinsic gain of the tube stage. Gain≈μ⋅​Rload​​/rp where µ = tube amplification factor and Rload = plate resistor, so 
        drive0 = tube gain + load interaction. higher → more saturation, lower → cleaner
- bias0 is the DC bias of the tube. Vgk ​= Vg ​− Vk​. Sets the operating point. 0 → symmetric distortion → odd harmonics, ≠0 → asymmetric → even harmonics
        this is critical for tube character
- tauF is the Fast time constant. Corresponds to grid capacitance, small RC networks. Effect: pick attack response, transient behavior, controls 
        "touch sensitivity"
- tauS is the Slow time constant. Corresponds to power supply filtering caps, B+ rail dynamics. Effect: sag, slow compression, gives the "spongy" feel.
- kF is the Drive modulation by fast envelope. drive = drive0 ​+ kF​⋅eF​. Corresponds to instantaneous nonlinear conduction, grid interaction.
        Effect: harder picking → more gain, attack-dependent distortion.
- kS is the Drive reduction from slow envelope. drive = drive0​ − kS​⋅eS​. Corresponds to voltage drop in power supplycurrent limitation. 
        Effect: compression sag, classic tube amp behavior.
- kB is the Bias shift with signal level. bias = bias0 ​+ kB​⋅eS​. Corresponds to bias shift, cathode resistor effects, grid current. 
        Effect: evolving asymmetry, dynamic harmonic balance, very realistic tube behavior
- biasEnvEnable is a switch to Enable/disable dynamic bias.
- postGain is the Output gain. Corresponds to the "next stage gain" or master volume. Ssed to compensate level changes

Tube nonlinearity → tanh
- Bias → biasOffset
- Sag → kS + tauS
- Attack dynamics → kF + tauF
- Bias shift → kB

This is a tentative to tube stage abstraction with a gray-box modeling apporach of the physical structure also with data-driven parameters
*/
dynamicTubeStage(inGain, drive0, bias0, tauF, tauS, kF, kS, kB, biasEnvEnable, postGain) = stage
with {
    stage(x) = y
    with {
        pre = x * inGain;

        // Etats lents
        eF = pre : envFollower(tauF);
        eS = pre : envFollower(tauS);

        // Drive dynamique
        driveDyn = clamp(0.05, 40.0, drive0 + kF * eF - kS * eS);

        // Bias dynamique optionnel
        biasDyn = clamp(-1.5, 1.5, bias0 + (biasEnvEnable * kB * eS));

        // Non-linéarité asymétrique
        sat = asymBiasedTanh(driveDyn, biasDyn, pre);

        // Sortie
        y = sat * postGain;
    };
};


// ============================================================
// Pre-configured tube instances  –  calibrated on actual UI ranges:
//   drive0   : 0.05..20  (UI default 2.2)
//   inGain   : 0.1..4    (UI default 1.0)
//   bias0    : -1..1     (UI default 0.0)
//   tauF     : 0.1..20 ms (UI default 3.0)
//   tauS     : 5..500 ms  (UI default 80.0)
//   kF       : -20..20   (UI default 3.0)
//   kS       : -20..20   (UI default 1.0)
//   kB       : -2..2     (UI default 0.15)
//   biasEnv  : 0=off / 1=on (UI default 0)
//   postGain : 0.01..4   (UI default 1.0)
//
// Tube character mapping:
//   12AX7 : highest drive, pronounced asymmetry, responsive dynamics
//   12AT7 : medium drive, cleaner headroom, less asymmetry
//   12AU7 : low drive, very clean, minimal asymmetry
//   6V6   : warm, strong sag (high kS, long tauS), round
//   6DJ8  : fast transient (short tauF), articulate, slight asymmetry
//   6C16  : aggressive, most asymmetric, high kF touch sensitivity
//
// Stage role:
//   Stage1 : input stage, most saturation, strongest dynamics
//   Stage2 : intermediate, moderate character
//   Stage3 : driver stage, cleanest, least drive
//
// DC-blocker at 10 Hz removes bias-induced DC offset.
// ============================================================

dynTube(inGain, d0, b0, tF, tS, kF, kS, kB, bEn, pG) =
    dynamicTubeStage(inGain, d0, b0, tF, tS, kF, kS, kB, bEn, pG)
    : fi.highpass(1, 10.0);

// --- 12AX7 (ECC83) : high gain, musical saturation, touch-sensitive ---
//              inG   d0    b0    tF   tS    kF    kS    kB   en    pG
T1_12AX7_dyn = dynTube(1.0, 5.5, -0.20, 3.0, 80.0,  4.0,  1.2,  0.20,  1, 0.90);
T2_12AX7_dyn = dynTube(1.0, 4.0, -0.16, 3.0, 80.0,  3.5,  1.0,  0.17,  1, 0.90);
T3_12AX7_dyn = dynTube(1.0, 3.0, -0.12, 3.0, 80.0,  3.0,  0.8,  0.14,  1, 0.95);

// --- 12AT7 (ECC81) : medium gain, clean headroom ---
T1_12AT7_dyn = dynTube(1.0, 4.0, -0.14, 3.0, 80.0,  3.5,  1.1,  0.16,  1, 0.90);
T2_12AT7_dyn = dynTube(1.0, 3.0, -0.11, 3.0, 80.0,  3.0,  0.9,  0.14,  1, 0.92);
T3_12AT7_dyn = dynTube(1.0, 2.4, -0.08, 3.0, 80.0,  2.5,  0.7,  0.12,  1, 0.95);

// --- 12AU7 (ECC82) : low gain, very clean, high headroom ---
T1_12AU7_dyn = dynTube(1.0, 2.5, -0.08, 3.0, 80.0,  2.5,  0.9,  0.12,  1, 0.95);
T2_12AU7_dyn = dynTube(1.0, 2.0, -0.06, 3.0, 80.0,  2.2,  0.7,  0.10,  1, 0.97);
T3_12AU7_dyn = dynTube(1.0, 1.8, -0.05, 3.0, 80.0,  2.0,  0.6,  0.08,  0, 1.00);

// --- 6V6 : warm, spongy sag, round saturation ---
T1_6V6_dyn   = dynTube(1.0, 2.2, -0.18, 3.0, 80.0, 3.0,  1,  0.22,  1, 0.85);
T2_6V6_dyn   = dynTube(1.0, 2.2, -0.14, 3.0, 80.0, 3.0,  1,  0.18,  1, 0.88);
T3_6V6_dyn   = dynTube(1.0, 2.2, -0.11, 3.0, 80.0, 3.0,  1,  0.15,  1, 0.92);

// --- 6DJ8 (ECC88) : fast, articulate, near-symmetric ---
T1_6DJ8_dyn  = dynTube(1.0, 4.5, -0.08, 1.5, 60.0,  4.5,  1.0,  0.10,  1, 0.90);
T2_6DJ8_dyn  = dynTube(1.0, 3.5, -0.06, 1.5, 60.0,  4.0,  0.8,  0.08,  1, 0.92);
T3_6DJ8_dyn  = dynTube(1.0, 2.8, -0.04, 1.5, 60.0,  3.5,  0.6,  0.07,  0, 0.95);

// --- 6C16 : aggressive, asymmetric, bright character ---
T1_6C16_dyn  = dynTube(1.0, 6.0, -0.28, 3.0, 80.0,  5.0,  1.5,  0.28,  1, 0.85);
T2_6C16_dyn  = dynTube(1.0, 4.8, -0.22, 3.0, 80.0,  4.5,  1.2,  0.24,  1, 0.87);
T3_6C16_dyn  = dynTube(1.0, 3.8, -0.17, 3.0, 80.0,  4.0,  1.0,  0.20,  1, 0.90);
