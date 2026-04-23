import("stdfaust.lib");
import("IFCdynTube.dsp");

declare name      "IFCpreampGridCurrentCathodeBiasOptionnalDynStage";
declare author    "Michel Buffa";
declare version   "1.0";
declare license   "LGPL";

/* Ceci est un "preamp lab" permettant de construire un preamp avec de nombreuses
   possibilités de réglages. 

   Deux modèles de lampes sont proposés :
   1 - un premier modèle "dynamique" (pentode), en trois parties distinctes:
   une partie modélisant le courant de grille (avant la pentode)
   une partie modélisant la pentode elle-même
   une partie modélisant le bias de cathode

   2 - un modèle "tout en un", avec des réglages théoriquement plus fins. 
   Voir détails dans le fichier IFCdynTube.dsp.

   Il est possible de choisir le nombre d'étages voulus, les paramètres des 
   lampes, les paramètres des filtres inter étages, du grid current, du cathode bias, 
   par étage etc. Des presets sont fournis pour un ensemble d'amplificateurs
   à lampes emblématiques.

   La UI permet de choisir soit tous les étages basés sur  le modèle 1 (menu "static"),
   soit tous les étages avec modèle 1 mais avec le dernier étage 
   utilisant le modèle 2 plus "dynamique".

   Optionnellement, on peut activer l'anti-aliasing ADAA sur les modèles de lampes,
   et des filtres hi-cut et lo-cut en sortie du preamp.

   On peut régler le gain avant le preamp, et un gain de sortie dans la UI.
   Un réglage du gain entre les étages 1 et 2 est également proposé dans la UI.
*/

// ------ PREAMP ----@-
input_vol = hgroup("2 Preamp", hslider("Input Vol[style:knob]", 1.0, 0.0, 3.0, 0.01)) : si.smoo;
gain_knob = hgroup("2 Preamp", hslider("Interstage Gain[style:knob]", 0.07, 0.01, 1.0, 0.01)) : si.smoo;
preamp_out = hgroup("2 Preamp", hslider("Master Vol[style:knob]", 1.0, 0.0, 5.0, 0.01)) : si.smoo;

// Interstage gain normalization: compensates for the 12× spread in sensitivity
// between presets (Fender=very reactive, Peavey=barely reactive to same gk).
// Computed from: a2 × tanh(a1 × 0.2) per preset, normalized to Custom mode.
//                Custom  Marshall  Fender  Mesa   Vox    Peavey  Deluxe  Soldano  Orange
gk_scale = (1.00,   0.43,    2.22,   0.24,  0.48,   0.19,   1.00,   0.24,   0.34)
         : ba.selectn(9, amp_preset) : si.smoo;

// gk_eff is used in all interstage connections instead of raw gain_knob
gk = gain_knob * gk_scale;

// Dynamic compensation: keeps output constant when gain_knob changes.
// Uses tanh-based model per preset: accounts for nonlinear saturation in S2.
//   - triode_a2 × gk_scale = per-preset sensitivity of S2 to gk
//   - tanh ratio: in linear regime → ≈ 0.07/gain_knob (clean amps)
//                 in saturated regime → ≈ 1.0 (hi-gain amps, output barely changes)
s2_eff_sens = triode_a2 * gk_scale;
gk_comp_ratio = ma.tanh(s2_eff_sens * 0.07)
              / max(0.001, ma.tanh(s2_eff_sens * max(0.001, gain_knob)));
// Disable for 1-stage mode (interstage gain not used)
gk_comp = select2(preamp_stages > 0, 1.0, gk_comp_ratio) : si.smoo;

// Master amp model selector — drives all sub-presets simultaneously
// 0=Custom (use individual UI sliders), 1-8 = specific amp model
// Indices: 0=Custom, 1=Marshall JCM800, 2=Fender Twin, 3=Mesa Dual Rect, 4=Vox AC30, 5=Peavey 5150, 6=Fender Deluxe, 7=Soldano SLO-100, 8=Orange Rockerverb
amp_preset = hgroup("2 Preamp", nentry("Amp Model[style:menu{'Custom':0;'Marshall JCM800':1;'Fender Twin Reverb':2;'Mesa Dual Rectifier':3;'Vox AC30 Top Boost':4;'Peavey 5150':5;'Fender Deluxe':6;'Soldano SLO-100':7;'Orange Rockerverb':8}]", 6, 0, 8, 1)) : int;

// ADAA Oversampling feature — uses Faust's official aa.tanh1 from aanl.lib
// (ADAA1 anti-aliased tanh with logcosh antiderivative, maintained by Grame)
os_enable_ui = hgroup("2 Preamp", checkbox("Oversampling MODEL 1]")) : int;
smart_tanh = _ <: select2(os_enable_ui, ma.tanh, aa.tanh1);

// -----------------------------------------------------------------------------
// Grid current / blocking-like models
// -----------------------------------------------------------------------------
gcClamp(lo, hi, x) = max(lo, min(hi, x));

gcEnvFollower(tauMs) = si.smooth(alpha)
with {
    alpha = gcClamp(0.0001, 0.9999, 1.0 / max(1.0, (tauMs * ma.SR * 0.001)));
};

// Grid conduction proxy: current appears only above threshold
gridCurrent(vth) = max(0.0, _ - vth);

// v2: returns modified input + bias shift proxy
gridCurrentBiasBlock(vth, amtIn, amtBias, tauMs) = _ <: inMod, biasShift
with {
    s = gridCurrent(vth) : gcEnvFollower(tauMs);
    inMod(x)     = x - amtIn * s(x);
    biasShift(x) = 0.0 - (amtBias * s(x));
};

// =============================================================================
// Grid Current Controls — presets par ampli
// =============================================================================
//
// PHYSIQUE DU PHÉNOMÈNE :
//   Quand Vgk > 0 (la grille devient positive), le tube conduit du courant
//   de grille à travers Rg. Cela :
//   (1) Charge le condensateur de couplage Cc → Vgk dérive vers le négatif
//       pendant la récupération (tau = Rg × Cc)
//   (2) Atténue le signal d'entrée (l'impédance d'entrée chute)
//   → résultat : "strangling" ou "blocking distortion" sur les transitoires forts
//
// PARAMÈTRES :
//   vth      : seuil normalisé de conduction grille (Vgk > 0)
//              élevé (0.7-0.8) → rarement déclenché (amplis clean)
//              bas  (0.2-0.35) → déclenché fréquemment (amplis hi-gain)
//   amtIn    : réduction de l'entrée par le courant de grille (0=aucun, 1=fort)
//   amtBias  : décalage de biais (compression/étranglement) (0=aucun, 1=fort)
//   tau (ms) : constante de temps de récupération = Rg × Cc
//              ex: 1MΩ × 100nF = 100ms, 220kΩ × 22nF ≈ 5ms
//              long tau → récupération lente → son "breathe" lent
//              court tau → récupération rapide → attaque serrée
//
// CARACTÈRE SONORE PAR AMPLI :
//   Fender Twin/Deluxe : enable=OFF, vth élevé → clean, effet quasi-absent
//   Marshall JCM800    : S2+S3 ON, vth~0.4 → étranglement modéré sur les peaks
//   Mesa Dual Rect.    : tous ON, vth~0.3 → fort, serre l'attaque, son dense
//   Vox AC30           : S2 ON, vth=0.5  → légère "respiration", contribue au chime
//   Peavey 5150        : tous ON, vth~0.25, tau court → très serré, agressif
//   Soldano SLO-100    : S2+S3 ON, modéré → smooth bloom, récupération lente
//   Orange Rockerverb  : tous ON, modéré  → "chunky", arrondi sur les transitoires
//
// NOTE : l'effet est d'autant plus audible que les étages suivants sont saturés
// =============================================================================

// amp_preset drives this — when 0 (Custom), the UI sliders below are used
gc_preset = amp_preset;

// -----------------------------------------------------------------------------
// Grid current Stage 1
// Physique : le premier étage est le moins sollicité, le signal d'entrée est faible
// → grid current peu probable sauf en amplification extrême (hi-gain)
// -----------------------------------------------------------------------------
gc1_enable_ui  = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 1]", checkbox("Enable]")));
gc1_vth_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 1]", hslider("Threshold]",   0.7,  0.05, 0.90, 0.01))) : si.smoo;
gc1_amtIn_ui   = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 1]", hslider("Input Push]",  0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc1_amtBias_ui = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 1]", hslider("Bias Push]",   0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc1_tau_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 1]", hslider("Recovery ms]", 80.0, 5.0, 500.0, 1.0)))  : si.smoo;

//              Custom        Marshall    Fender    Mesa   Vox  Peavey  Deluxe Soldano Orange
grid1_enable  = (gc1_enable_ui,  0,         0,      1,      0,     1,    0,     0,     1)     : ba.selectn(9, gc_preset) : si.smoo;
grid1_vth     = (gc1_vth_ui,     0.5,       0.80,   0.40,   0.60,  0.35, 0.7,   0.45,  0.45)  : ba.selectn(9, gc_preset) : si.smoo;
grid1_amtIn   = (gc1_amtIn_ui,   0.10,      0.02,   0.15,   0.08,  0.20, 0.05,  0.15,  0.12)  : ba.selectn(9, gc_preset) : si.smoo;
grid1_amtBias = (gc1_amtBias_ui, 0.10,      0.02,   0.15,   0.08,  0.20, 0.05,  0.15,  0.12)  : ba.selectn(9, gc_preset) : si.smoo;
grid1_tau     = (gc1_tau_ui,     40.0,      120.0,  30.0,   60.0,  25.0, 80.0,  40.0,  40.0)  : ba.selectn(9, gc_preset) : si.smoo;

// -----------------------------------------------------------------------------
// Grid current Stage 2
// Physique : le signal est plus fort (post-stage1), le grid current est plus probable
// → activation sur Marshall, Mesa, Peavey, Soldano, Orange
// -----------------------------------------------------------------------------
gc2_enable_ui  = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 2]", checkbox("Enable]")));
gc2_vth_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 2]", hslider("Threshold]",   0.7,  0.05, 0.90, 0.01))) : si.smoo;
gc2_amtIn_ui   = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 2]", hslider("Input Push]",  0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc2_amtBias_ui = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 2]", hslider("Bias Push]",   0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc2_tau_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 2]", hslider("Recovery ms]", 80.0, 5.0, 500.0, 1.0)))  : si.smoo;

//               Custom        Marshall   Fender  Mesa     Vox  Peavey  Deluxe Soldano Orange
grid2_enable  = (gc2_enable_ui,  1,         0,      1,      1,     1,    0,     1,     1)     : ba.selectn(9, gc_preset) : si.smoo;
grid2_vth     = (gc2_vth_ui,     0.4,       0.80,   0.30,   0.50,  0.25, 0.7,   0.35,  0.35)  : ba.selectn(9, gc_preset) : si.smoo;
grid2_amtIn   = (gc2_amtIn_ui,   0.20,      0.02,   0.40,   0.15,  0.50, 0.05,  0.35,  0.25)  : ba.selectn(9, gc_preset) : si.smoo;
grid2_amtBias = (gc2_amtBias_ui, 0.20,      0.02,   0.40,   0.15,  0.45, 0.05,  0.35,  0.25)  : ba.selectn(9, gc_preset) : si.smoo;
grid2_tau     = (gc2_tau_ui,     60.0,      120.0,  50.0,   80.0,  40.0, 80.0,  60.0,  60.0)  : ba.selectn(9, gc_preset) : si.smoo;

// -----------------------------------------------------------------------------
// Grid current Stage 3
// Physique : signal le plus fort → le plus susceptible de déclencher le courant
// → activation sur la plupart des amplis hi-gain
// -----------------------------------------------------------------------------
gc3_enable_ui  = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 3]", checkbox("Enable]")));
gc3_vth_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 3]", hslider("Threshold]",   0.7,  0.05, 0.90, 0.01))) : si.smoo;
gc3_amtIn_ui   = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 3]", hslider("Input Push]",  0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc3_amtBias_ui = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 3]", hslider("Bias Push]",   0.05, 0.0,  1.0,  0.01))) : si.smoo;
gc3_tau_ui     = hgroup("2 Preamp/1 Grid Current (MODEL 1)", vgroup("Stage 3]", hslider("Recovery ms]", 80.0, 5.0, 500.0, 1.0)))  : si.smoo;

//               Custom        Marshall   Fender   Mesa    Vox  Peavey Deluxe Soldano Orange
grid3_enable  = (gc3_enable_ui,  1,         0,      1,      0,     1,    0,     1,     1)     : ba.selectn(9, gc_preset) : si.smoo;
grid3_vth     = (gc3_vth_ui,     0.35,      0.80,   0.25,   0.50,  0.20, 0.7,   0.30,  0.30)  : ba.selectn(9, gc_preset) : si.smoo;
grid3_amtIn   = (gc3_amtIn_ui,   0.30,      0.02,   0.50,   0.15,  0.60, 0.05,  0.45,  0.35)  : ba.selectn(9, gc_preset) : si.smoo;
grid3_amtBias = (gc3_amtBias_ui, 0.30,      0.02,   0.50,   0.15,  0.55, 0.05,  0.45,  0.35)  : ba.selectn(9, gc_preset) : si.smoo;
grid3_tau     = (gc3_tau_ui,     100.0,     120.0,  80.0,   80.0,  60.0, 80.0,  90.0,  80.0)  : ba.selectn(9, gc_preset) : si.smoo;


// -----------------------------------------------------------------------------
// Cathode Bias Controls
// -----------------------------------------------------------------------------
cathode_shift(tau, scale, x) = x : max(0.0) : si.smooth(1.0 / max(1.0, tau * ma.SR * 0.001)) : *(scale);

// =============================================================================
// Cathode Bias Controls — presets par ampli
// =============================================================================
//
// PHYSIQUE DU PHÉNOMÈNE :
//   Le condensateur de bypass de cathode Ck charge quand Vk monte avec le signal.
//   Quand on joue fort, la tension cathode monte (Vk ↑) → Vgk ↓ → baisse du
//   gain et décalage du point de fonctionnement. C'est la principale source de
//   "sag" et de toucher dynamique dans un ampli à lampes.
//   Dans le modèle : cathode_shift ne prend que les demi-cycles positifs (max(0))
//   et les lisse par RC → simule la charge lente de Ck.
//
// PARAMÈTRES :
//   tau (ms) = constante de temps RC = Rk × Ck
//              ex : Rk=2700Ω, Ck=25µF → τ = 67.5ms  (Marshall stage 1)
//              ex : Rk= 820Ω, Ck=25µF → τ = 20.5ms  (stage driver, plus tight)
//              Long tau → sag lent, "breathing" musical
//              Court tau → récupération rapide, attaque serrée
//   scale    = amplitude normalisée du décalage de biais (0=aucun, 0.5=très fort)
//              Au-delà de 0.5 le point de fonctionnement quitte la zone linéaire de tanh
//
// CARACTÈRE SONORE PAR AMPLI :
//   Fender Twin/Deluxe : tau long (25-68ms), scale faible (0.06-0.10) → sag doux
//   Marshall JCM800    : tau moyen (20-68ms), scale modéré (0.12-0.22) → feel classique
//   Mesa Dual Rect.    : tau court (12-20ms), scale modéré (0.15-0.28) → tight/compressé
//   Vox AC30 (Class A) : tau long (50-60ms), scale élevé (0.18-0.22) → forte sensibilité
//   Peavey 5150        : tau très court (8-15ms), scale fort (0.20-0.38) → très serré
//   Soldano SLO-100    : tau moyen (20-50ms), scale modéré → smooth, bloom progressif
//   Orange Rockerverb  : tau long (25-60ms), scale modéré → organique, chaleureux
//
// NOTE : S3 désactivé pour Fender Deluxe et Fender Twin car un vrai Deluxe/Twin
//   n'a que 2 étages de triode préampli avant le phase inverter.
// =============================================================================

// amp_preset drives this — when 0 (Custom), the UI sliders below are used
cath_preset = amp_preset;

// -----------------------------------------------------------------------------
// Cathode Bias Stage 1
// Physique : Rk élevé (1500-2700Ω), Ck≈25µF → tau=37-68ms. Sag lent.
// -----------------------------------------------------------------------------
cath1_en_ui    = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 1]", checkbox("Enable]")));
cath1_tau_ui   = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 1]", hslider("RC Time (ms)]", 40.0,  5.0, 300.0, 1.0))) : si.smoo;
cath1_scale_ui = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 1]", hslider("Shift Factor]",  0.08,  0.0,   0.5, 0.01))) : si.smoo;

//             Custom       Marshall     Fender    Mesa   Vox   Peavey  Deluxe Soldano Orange
cath1_en    = (cath1_en_ui,    1,           1,       1,     1,    1,     1,     1,     1)   : ba.selectn(9, cath_preset);
cath1_tau   = (cath1_tau_ui,  68.0,        40.0,    20.0,  60.0, 15.0,  40.0,  50.0,  60.0): ba.selectn(9, cath_preset) : si.smoo;
cath1_scale = (cath1_scale_ui, 0.12,        0.08,    0.15,  0.18, 0.20,  0.08,  0.12,  0.12): ba.selectn(9, cath_preset) : si.smoo;

// -----------------------------------------------------------------------------
// Cathode Bias Stage 2
// Physique : Rk moyen (820-1500Ω), Ck≈25µF → tau=20-38ms. Signal plus fort.
// -----------------------------------------------------------------------------
cath2_en_ui    = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 2]", checkbox("Enable]")));
cath2_tau_ui   = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 2]", hslider("RC Time (ms)]", 25.0,  5.0, 300.0, 1.0))) : si.smoo;
cath2_scale_ui = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 2]", hslider("Shift Factor]",  0.10,  0.0,   0.5, 0.01))) : si.smoo;

//             Custom       Marshall      Fender  Mesa    Vox   Peavey  Deluxe Soldano Orange
cath2_en    = (cath2_en_ui,    1,           1,       1,     1,    1,     1,     1,     1)   : ba.selectn(9, cath_preset);
cath2_tau   = (cath2_tau_ui,  38.0,        25.0,    15.0,  60.0, 10.0,  25.0,  30.0,  35.0): ba.selectn(9, cath_preset) : si.smoo;
cath2_scale = (cath2_scale_ui, 0.18,        0.08,    0.22,  0.22, 0.30,  0.10,  0.20,  0.20): ba.selectn(9, cath_preset) : si.smoo;

// -----------------------------------------------------------------------------
// Cathode Bias Stage 3
// Physique : Rk faible (470-820Ω) → tau court. Stage driver très sollicité.
// Désactivé pour Fender Deluxe/Twin (pas de 3e triode préampli dans le circuit réel)
// -----------------------------------------------------------------------------
cath3_en_ui    = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 3]", checkbox("Enable]")));
cath3_tau_ui   = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 3]", hslider("RC Time (ms)]", 25.0,  5.0, 300.0, 1.0))) : si.smoo;
cath3_scale_ui = hgroup("2 Preamp/3 Cathode Bias (MODEL 1)", vgroup("Stage 3]", hslider("Shift Factor]",  0.08,  0.0,   0.5, 0.01))) : si.smoo;

//             Custom       Marshall     Fender     Mesa   Vox Peavey  Deluxe Soldano Orange
cath3_en    = (cath3_en_ui,    1,           0,       1,     1,    1,     0,     1,     1)   : ba.selectn(9, cath_preset);
cath3_tau   = (cath3_tau_ui,  20.0,        25.0,    12.0,  50.0,  8.0,  25.0,  20.0,  25.0): ba.selectn(9, cath_preset) : si.smoo;
cath3_scale = (cath3_scale_ui, 0.22,        0.06,    0.28,  0.18, 0.38,  0.08,  0.25,  0.25): ba.selectn(9, cath_preset) : si.smoo;


// -----------------------------------------------------------------------------
// Triode Characteristics Controls
// -----------------------------------------------------------------------------
// amp_preset drives this — when 0 (Custom), the UI sliders below are used
triode_preset = amp_preset;

triode_a1_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V1 Drive (a)]", 4.5, 0.5, 15.0, 0.01));
triode_b1_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V1 Bias (b)]", -0.08, -0.50, 0.05, 0.001));

triode_a2_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V2 Drive (a)]", 2.5, 0.5, 15.0, 0.01));
triode_b2_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V2 Bias (b)]", -0.10, -0.50, 0.05, 0.001));

triode_a3_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V3 Drive (a)]", 3.0, 0.5, 15.0, 0.01));
triode_b3_ui = hgroup("2 Preamp/2 Triode Characteristics (MODEL 1)", hslider("V3 Bias (b)]", -0.10, -0.50, 0.05, 0.001));

// PRESETS DES VALEURS DES PARAMETRES DES LAMPES, ETAGES "STATIQUES"
// ---------------------
//Valeurs selon le preset (V1 Drive) : 
// Effet audible : plus a1 est grand, plus le signal entre dans tanh avec 
// amplitude élevée → la saturation s'enclenche plus tôt → plus de distorsion 
// harmonique à volume équivalent.
// 
// ── PRESETS — Caractère sonore par ampli ──────────────────────────────────────
//
// a = gain pré-saturation (plus grand → saturation plus précoce et plus forte)
// b = biais (plus négatif → plus d'harmoniques pairs H2 → son plus "chaud/tubey")
//
// NOTE pour JEROME: les valeurs a2 d'origine (59–85) quand tu es passé à mon bureau
//  étaient trop élevées : elles provoquaient une saturation totale permanente 
//  même à faible niveau. Les nouvelles valeurs (1.5–10) permettent une progression 
//  réaliste dans les 3 étages.
//
//  ┌─────────────────────────────────────────────────────────────────────────────┐
//  │ MARSHALL JCM800 (idx=1)                                                     │
//  │ Son : crunch mid-gain, médiums présents, compression progressive.           │
//  │ a1=6   b1=-0.12  Stage 1 modéré, légère asymétrie                          │
//  │ a2=5   b2=-0.16  Stage 2 sature en jouant fort, H2 croissant               │
//  │ a3=3.5 b3=-0.15  Stage 3 driver, asymétrie stable                          │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ FENDER TWIN REVERB (idx=2)                                                  │
//  │ Son : cristallin, headroom maximal, distorsion seulement à volume extrême.  │
//  │ a1=3   b1=-0.04  Très peu de gain, quasi-linéaire                          │
//  │ a2=1.5 b2=-0.06  Reste propre même à plein volume                          │
//  │ a3=2   b3=-0.06  Légère couleur "glassy"                                   │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ MESA DUAL RECTIFIER (idx=3)                                                 │
//  │ Son : high-gain serré, forte compression, asymétrie prononcée.             │
//  │ a1=8   b1=-0.10  Premier étage actif                                        │
//  │ a2=8   b2=-0.30  Asymétrie forte (H2 dominant), son dense/serré            │
//  │ a3=6   b3=-0.20  Driver sature aussi, son très compressé                   │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ VOX AC30 TOP BOOST (idx=4)                                                  │
//  │ Son : "chime" brillant, harmoniques pairs musicaux, réactif au toucher.    │
//  │ a1=6   b1=-0.08  Gain présent mais pas extrême, légère asymétrie réelle   │
//  │ a2=4.5 b2=-0.07  L'essentiel du "chime" vient du power amp Class A        │
//  │ a3=3.5 b3=-0.06  Driver propre                                             │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ PEAVEY 5150 (idx=5)                                                         │
//  │ Son : high-gain agressif, attaque brutale, graves serrés.                  │
//  │ a1=9   b1=-0.14  Fort gain stage 1, asymétrie dès l'entrée                │
//  │ a2=10  b2=-0.24  Gain le plus élevé de tous sur stage 2                   │
//  │ a3=8   b3=-0.18  Stage 3 aussi saturé → compression maximale              │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ FENDER DELUXE (idx=6)                                                       │
//  │ Son : entre Twin et Marshall, légèrement breakup à plein volume.            │
//  │ a1=4.5 b1=-0.08  Plus chaud que le Twin, léger H2                         │
//  │ a2=2.5 b2=-0.10  Breakup doux en jouant fort                               │
//  │ a3=3   b3=-0.10  Son "vintage" équilibré                                   │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ SOLDANO SLO-100 (idx=7)                                                     │
//  │ Son : high-gain "smooth", très gainé mais sans agressivité excessive.      │
//  │ a1=9   b1=-0.12  Fort gain entrée, caractère marqué                        │
//  │ a2=8   b2=-0.22  Haut gain, asymétrie modérée → saturation "douce"        │
//  │ a3=6   b3=-0.18  Driver riche en harmoniques                              │
//  ├─────────────────────────────────────────────────────────────────────────────┤
//  │ ORANGE ROCKERVERB (idx=8)                                                   │
//  │ Son : medium-high gain, "chunky", rondeur dans le bas-médium.              │
//  │ a1=7   b1=-0.10  Gain équilibré, asymétrie naturelle                       │
//  │ a2=6   b2=-0.14  Saturation progressive, pas de compression excessive      │
//  │ a3=5   b3=-0.12  Son "organique"                                           │
//  └─────────────────────────────────────────────────────────────────────────────┘
//
// Routing via ba.selectn
// Index:  0=Custom,   1=Marshall, 2=Fender, 3=Mesa, 4=Vox,  5=Peavey, 6=Deluxe, 7=Soldano, 8=Orange
triode_a1 = (triode_a1_ui,  6.0,   3.0,   8.0,  6.0,   9.0,    4.5,   9.0,   7.0)  : ba.selectn(9, triode_preset) : si.smoo;
triode_b1 = (triode_b1_ui, -0.12, -0.04, -0.10, -0.08, -0.14, -0.08, -0.12, -0.10) : ba.selectn(9, triode_preset) : si.smoo;

triode_a2 = (triode_a2_ui,  5.0,   1.5,   8.0,  4.5,   10.0,  2.5,   8.0,   6.0)   : ba.selectn(9, triode_preset) : si.smoo;
triode_b2 = (triode_b2_ui, -0.16, -0.06, -0.30, -0.07, -0.24, -0.10, -0.22, -0.14) : ba.selectn(9, triode_preset) : si.smoo;

triode_a3 = (triode_a3_ui,  3.5,   2.0,   6.0,  3.5,    8.0,   3.0,   6.0,   5.0)  : ba.selectn(9, triode_preset) : si.smoo;
triode_b3 = (triode_b3_ui, -0.15, -0.06, -0.20, -0.06, -0.18, -0.10, -0.18, -0.12) : ba.selectn(9, triode_preset) : si.smoo;


// -----------------------------------------------------------------------------
// Dynamic controls
// -----------------------------------------------------------------------------
tube_mode = hgroup(
    "2 Preamp",
    nentry("Last stage model (MODEL 2)[style:menu{'Static':0;'Dynamic (Match Amp Preset)':1;'Dynamic (Manual Knobs)':2}]", 0, 0, 2, 1)
) : int;

is_dyn = tube_mode > 0;
is_dyn_f = float(is_dyn);

// -----------------------------------------------------------------------------
// Dynamic Tube Stage — presets par ampli
// -----------------------------------------------------------------------------
// Sliders Custom (index 0 dans les tables)
dyn_inScale_c       = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[1]In Scale[style:knob]",  1.32, 0.05,  2.0,  0.01)) : si.smoo;
dyn_tauF_c          = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[2]Attack[style:knob]",    2.0,  0.5, 15.0,  0.1))  : si.smoo;
dyn_tauS_c          = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[3]Release[style:knob]", 120.0, 10.0,500.0,  1.0))  : si.smoo;
dyn_drive0_c        = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[4]Drive[style:knob]",        2.2,  0.5, 15.0,  0.01)) : si.smoo;
dyn_biasOffset_c    = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[5]Bias[style:knob]", -0.153,-0.5,  0.3, 0.001)) : si.smoo;
dyn_kF_c            = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[6]DynF[style:knob]",    1.5, -5.0,  8.0,  0.01)) : si.smoo;
dyn_kS_c            = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[7]DynS[style:knob]",    0.8, -3.0,  5.0,  0.01)) : si.smoo;
dyn_kB_c            = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[8]DynB[style:knob]",    0.12,-1.0,  1.0, 0.001)) : si.smoo;
dyn_biasEnvEn_c     = hgroup("1 Dynamic Tube (MODEL 2)", checkbox("[81]BiasEnv[hidden:1]")) : int : si.smoo;
dyn_postGain_c      = hgroup("1 Dynamic Tube (MODEL 2)", hslider("[9]Out Gain[style:knob]",        1.375,  0.1,  3.0, 0.001)) : si.smoo;

// If tube_mode == 2 (Manual Knobs), force preset index to 0 (Custom) so sliders are active.
// Otherwise (tube_mode == 1), follow the global amp_preset.
dyn_preset = select2(tube_mode == 2, amp_preset, 0);

dyn_drive0_ui        = (dyn_drive0_c,     5.0,  2.5,  6.0,  4.0,  7.0,  3.0,  5.5,  4.5) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_inScale_ui       = (dyn_inScale_c,    0.35, 0.20, 0.40, 0.30, 0.45, 0.20, 0.35, 0.32) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_biasOffset_ui    = (dyn_biasOffset_c,-0.12,-0.05,-0.15,-0.08,-0.10,-0.08,-0.12,-0.10) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_tauF_ui          = (dyn_tauF_c,       2.0,  3.5,  1.5,  4.0,  1.0,  3.0,  2.0,  2.5) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_tauS_ui          = (dyn_tauS_c,     100.0,150.0, 80.0,200.0, 60.0,130.0,110.0,120.0) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_kF_ui            = (dyn_kF_c,         2.0,  0.5,  2.5,  1.0,  3.0,  0.8,  1.5,  1.5) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_kS_ui            = (dyn_kS_c,         1.0,  0.5,  1.5,  0.5,  2.0,  0.6,  1.0,  0.8) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_kB_ui            = (dyn_kB_c,         0.15, 0.06, 0.18, 0.25, 0.12, 0.10, 0.14, 0.15) : ba.selectn(9, dyn_preset) : si.smoo;
dyn_biasEnvEnable_ui = (dyn_biasEnvEn_c,  1,    1,    1,    1,    1,    1,    1,    1)    : ba.selectn(9, dyn_preset) : si.smoo;
dyn_postGain_ui      = (dyn_postGain_c,   1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0) : ba.selectn(9, dyn_preset) : si.smoo;


dyn_drive0_eff        = dyn_drive0_ui;
dyn_inScale_eff       = (1.0 - is_dyn_f) * 1.0 + is_dyn_f * dyn_inScale_ui;
dyn_biasOffset_eff    = is_dyn_f * dyn_biasOffset_ui;
dyn_tauF_eff          = (1.0 - is_dyn_f) * 3.0 + is_dyn_f * dyn_tauF_ui;
dyn_tauS_eff          = (1.0 - is_dyn_f) * 80.0 + is_dyn_f * dyn_tauS_ui;
dyn_kF_eff            = is_dyn_f * dyn_kF_ui;
dyn_kS_eff            = is_dyn_f * dyn_kS_ui;
dyn_kB_eff            = is_dyn_f * dyn_kB_ui;
dyn_biasEnvEnable_eff = is_dyn_f * dyn_biasEnvEnable_ui;
dyn_postGain_eff      = (1.0 - is_dyn_f) * 1.0 + is_dyn_f * dyn_postGain_ui;

safeDrive = max(0.05, dyn_drive0_eff);

// --- Preamp Monitoring (4 Junction Points) ---
pk_decay = exp(-1.0 / (0.5 * ma.SR));
p_hold = max ~ (*(pk_decay));

mtr_hb1 = hbargraph("0 Preamp Monitoring/[0]Input[unit:dB]", -60, 10);
mtr_hb2 = hbargraph("0 Preamp Monitoring/[1]Inter 1-2[unit:dB]", -60, 10);
mtr_hb3 = hbargraph("0 Preamp Monitoring/[2]Inter 2-3[unit:dB]", -60, 10);
mtr_hb4 = hbargraph("0 Preamp Monitoring/[3]Preamp Out[unit:dB]", -60, 10);

m1(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb1);
m2(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb2);
m3(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb3);
m4(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb4);

// -----------------------------------------------------------------------------
// Stage-specific helpers with grid current v2
// -----------------------------------------------------------------------------
// Chaque étage peut être statique ou dynamique.
// 
// 1 - Modèle statique, ici typiquement l'étage 1 (les autres reprennent le même modèle)
// ------------------------------------------
// stage1_static_local(z, bGc_in) =
//           z
//        :  *(a1)
//        :  +(b1 + bGc_in - cath_bias)
//        :  smart_tanh
//    with {
//        cath_bias = cath1_en * cathode_shift(cath1_tau, cath1_scale, z);
//    };
//
// Le signal z traverse 4 transformations en série :
//  - gain d'entrée a1: facteur de gain pré-saturation. Représente 
//      le produit µ × Rload / rp d'une triode — le gain en tension de 
//      l'étage avant saturation.  
//      Plus a1 est grand, plus le signal entre dans tanh avec amplitude 
//      élevée → la saturation s'enclenche plus tôt → plus de distorsion 
//                                                  harmonique à volume équivalent.
//  - offset de polarisation b1 + bGc_in - cath_bias 
//      C'est le "point de fonctionnement (bias)" de la triode. Décale le signal 
//      avant tanh. 
//      Représente la tension Vgk au point de repos, normalisée à 
//      l'amplitude du signal.

//      b1 < 0 → tanh asymétrique → harmoniques pairs (H2) → son "chaud", 
//          typique tube.
//      b1 = 0 → tanh symétrique → harmoniques impairs seulement (H3, H5...) 
//          → son plus "transistor"
//      Plus |b1| est grand → plus l'asymétrie est marquée
//
//      bGc_in : Décalage du courant de grille. Vient du bloc 
//          gridCurrentBiasBlock (quand activé). Quand le tube entre en 
//          conduction de grille (signal très fort), il génère un courant inverse 
//          qui ajoute un bias négatif dynamique → blocking distortion. 
//          Ce terme l'injecte dans le bias.

stage1_static = 
    _ <: (off_block, gc_block) 
      : (+, _) 
      : stage1_static_local
with {
    off_block = *(1.0 - grid1_enable);
    gc_block  = *(grid1_enable) : gridCurrentBiasBlock(grid1_vth, grid1_amtIn, grid1_amtBias, grid1_tau);

    a1 = triode_a1;
    b1 = triode_b1;

    stage1_static_local(z, bGc_in) =
           z
        :  *(a1)
        :  +(b1 + bGc_in - cath_bias)
        :  smart_tanh
    with {
        cath_bias = cath1_en * cathode_shift(cath1_tau, cath1_scale, z);
    };
};

stage1_or_dyn = 
    _ <: (off_block, gc_block) 
      : (+, _) 
      <: (stage1_static_local, stage1_dynamic_local) 
      : ba.selectn(2, is_dyn)
with {
    off_block = *(1.0 - grid1_enable);
    gc_block  = *(grid1_enable) : gridCurrentBiasBlock(grid1_vth, grid1_amtIn, grid1_amtBias, grid1_tau);

    a1 = triode_a1;
    b1 = triode_b1;

    stage1_static_local(z, bGc_in) =
           z
        :  *(a1)
        :  +(b1 + bGc_in - cath_bias)
        :  smart_tanh
    with {
        cath_bias = cath1_en * cathode_shift(cath1_tau, cath1_scale, z);
    };

    stage1_dynamic_local(z, bGc_in) =
        dynamicTubeStage(
            dyn_inScale_eff,
            dyn_drive0_eff,
            dyn_biasOffset_eff + bGc_in,
            dyn_tauF_eff,
            dyn_tauS_eff,
            dyn_kF_eff,
            dyn_kS_eff,
            dyn_kB_eff,
            dyn_biasEnvEnable_eff,
            dyn_postGain_eff
        )(z);
};

stage2_static = 
    _ <: (off_block, gc_block) 
      : (+, _) 
      : stage2_static_local
with {
    off_block = *(1.0 - grid2_enable);
    gc_block  = *(grid2_enable) : gridCurrentBiasBlock(grid2_vth, grid2_amtIn, grid2_amtBias, grid2_tau);

    a2 = triode_a2;
    b2 = triode_b2;

    stage2_static_local(z, bGc_in) =
           z
        :  *(a2)
        :  +(b2 + bGc_in - cath_bias)
        :  smart_tanh
    with {
        cath_bias = cath2_en * cathode_shift(cath2_tau, cath2_scale, z);
    };
};

stage2_or_dyn = 
    _ <: (off_block, gc_block) 
      : (+, _) 
      <: (stage2_static_local, stage2_dynamic_local) 
      : ba.selectn(2, is_dyn)
with {
    off_block = *(1.0 - grid2_enable);
    gc_block  = *(grid2_enable) : gridCurrentBiasBlock(grid2_vth, grid2_amtIn, grid2_amtBias, grid2_tau);

    a2 = triode_a2;
    b2 = triode_b2;

    stage2_static_local(z, bGc_in) =
           z
        :  *(a2)
        :  +(b2 + bGc_in - cath_bias)
        :  smart_tanh
    with {
        cath_bias = cath2_en * cathode_shift(cath2_tau, cath2_scale, z);
    };

    stage2_dynamic_local(z, bGc_in) =
        dynamicTubeStage(
            dyn_inScale_eff,
            dyn_drive0_eff,
            dyn_biasOffset_eff + bGc_in,
            dyn_tauF_eff,
            dyn_tauS_eff,
            dyn_kF_eff,
            dyn_kS_eff,
            dyn_kB_eff,
            dyn_biasEnvEnable_eff,
            dyn_postGain_eff
        )(z);
};

stage3_or_dyn = 
    _ <: (off_block, gc_block) 
      : (+, _) 
      <: (stage3_static_local, stage3_dynamic_local) 
      : ba.selectn(2, is_dyn)
with {
    off_block = *(1.0 - grid3_enable);
    gc_block  = *(grid3_enable) : gridCurrentBiasBlock(grid3_vth, grid3_amtIn, grid3_amtBias, grid3_tau);

    a3 = triode_a3;
    b3 = triode_b3;

    stage3_static_local(z, bGc_in) =
           z
        :  *(a3)
        :  +(b3 + bGc_in - cath_bias)
        :  smart_tanh
    with {
        cath_bias = cath3_en * cathode_shift(cath3_tau, cath3_scale, z);
    };

    stage3_dynamic_local(z, bGc_in) =
        dynamicTubeStage(
            dyn_inScale_eff,
            dyn_drive0_eff,
            dyn_biasOffset_eff + bGc_in,
            dyn_tauF_eff,
            dyn_tauS_eff,
            dyn_kF_eff,
            dyn_kS_eff,
            dyn_kB_eff,
            dyn_biasEnvEnable_eff,
            dyn_postGain_eff
        )(z);
};

// -----------------------------------------------------------------------------
// Topology
// -----------------------------------------------------------------------------
// Number of preamp stages driven by amp_preset (0=1stage, 1=2stages, 2=3stages)
// Marshall=3, Fender Twin=2, Mesa=3, Vox=2, Peavey=3, Deluxe=2, Soldano=3, Orange=3
preamp_stages_ui = hgroup(
    "2 Preamp",
    nentry("Stages[style:menu{'1 stage':0;'2 stages':1;'3 stages':2}]", 1, 0, 2, 1)
) : int;
//                    Custom             Marshall Fender  Mesa  Vox   Peavey  Deluxe Soldano Orange
preamp_stages = (preamp_stages_ui, 2,       1,       2,    1,    2,     1,      2,     2) : ba.selectn(9, amp_preset);


// =============================================================================
// Per-preset output level compensation
// =============================================================================
// Each amp model produces very different output levels due to varying drive
// values and number of stages. This table normalises the output so all presets
// have approximately the same perceived loudness with neutral input_vol=1,
// interstage gain=0.8 (default) and master=1.
//
// Reasoning (with input_vol=1, gk=0.8, preamp_out=1):
//   - tanh(a*x) output ≈ a*x when x is small (linear region)
//   - tanh saturates at ±1 for large |a*x|
//   - Clean amps (Fender Twin: a1=3, a2=1.5, 2 stages) produce low RMS
//   - Hi-gain amps (Peavey: a1=9…a3=8, 3 stages) sit at tanh≈±1 → high RMS
//   - Compensation = inverse of typical RMS at unity settings
//
// Values: >1 = boost (clean amps need more level)
//         <1 = cut   (hi-gain amps are already loud)
//         1  = neutral (Custom mode, user controls everything)
//
//                   Custom  Marshall  Fender  Mesa   Vox    Peavey  Deluxe  Soldano  Orange
// Analytically recalibrated from simulated RMS output (440Hz sine, A=0.2, with gk_scale applied)
preamp_level_comp = (1.00,   0.23,     1.27,   0.15,  1.16,  0.15,   1.00,   0.17,    0.21)
                  : ba.selectn(9, amp_preset) : si.smoo;


// amp_preset drives this — when 0 (Custom), the UI sliders below are used
filter_preset = amp_preset;

hp12_ui = hgroup("2 Preamp/4 Interstage Filters", hslider("HPF 1-2 (Hz)]",  15.0,   5.0,  500.0,  1.0)) : si.smoo;
lp12_ui = hgroup("2 Preamp/4 Interstage Filters", hslider("LPF 1-2 (Hz)]", 10000.0, 2000.0, 18000.0, 10.0)) : si.smoo;
hp23_ui = hgroup("2 Preamp/4 Interstage Filters", hslider("HPF 2-3 (Hz)]",  20.0,   5.0,  500.0,  1.0)) : si.smoo;
lp23_ui = hgroup("2 Preamp/4 Interstage Filters", hslider("LPF 2-3 (Hz)]",  8000.0, 2000.0, 18000.0, 10.0)) : si.smoo;

// Index: 0        1         2        3        4        5        6        7        8
hp12 = (hp12_ui, 60.0,    30.0,     10.0,    150.0,   120.0,   15.0,    80.0,    30.0)    : ba.selectn(9, filter_preset) : si.smoo;
lp12 = (lp12_ui, 9418.0,  12000.0,  8000.0,  15000.0, 9000.0,  10000.0, 8500.0,  6500.0)  : ba.selectn(9, filter_preset) : si.smoo;
hp23 = (hp23_ui, 79.0,    15.0,     5.0,     40.0,    180.0,   20.0,    100.0,   20.0)    : ba.selectn(9, filter_preset) : si.smoo;
lp23 = (lp23_ui, 12180.0, 10000.0,  6500.0,  10000.0, 7500.0,  8000.0,  7000.0,  5500.0)  : ba.selectn(9, filter_preset) : si.smoo;

interStage12 = fi.highpass(1, hp12) : fi.lowpass(1, lp12);
interStage23 = fi.highpass(1, hp23)  : fi.lowpass(1, lp23);
interstage_dc = fi.dcblocker;

// Preamp sequential logic blocks
s1_block = (_ <: stage1_static, stage1_or_dyn : select2(preamp_stages == 0)) : m2;
s2_block = interstage_dc : *(gk) : interStage12 
         : (_ <: stage2_static, stage2_or_dyn : select2(preamp_stages == 1)) : m3;
s3_block = interstage_dc : interStage23 : stage3_or_dyn : m4;

// -----------------------------------------------------------------------------
// Post-Preamp Sculpting EQ (hi and lo cuts)
// -----------------------------------------------------------------------------
hc_en = hgroup("3 Post-Preamp EQ", checkbox("High Cut]"));
lc_en = hgroup("3 Post-Preamp EQ", checkbox("Low Cut]"));

post_eq_group(x) = hgroup("3 Post-Preamp EQ", 
    x : (
        _ <: _, fi.peak_eq_cq(
            hslider("High Cut Gain (dB)]", -25.0, -80.0, 20.0, 0.1) : si.smoo,
            hslider("High Cut Freq (Hz)]", 10000.0, 1000.0, 20000.0, 10.0) : si.smoo,
            hslider("High Cut Q]", 1.0, 0.1, 10.0, 0.01) : si.smoo
        ) : select2(hc_en)
    ) :
    (
        _ <: _, fi.peak_eq_cq(
            hslider("Low Cut Gain (dB)]", -19.0, -80.0, 20.0, 0.1) : si.smoo,
            hslider("Low Cut Freq (Hz)]", 60.0, 10.0, 1000.0, 1.0) : si.smoo,
            hslider("Low Cut Q]", 1.0, 0.1, 10.0, 0.01) : si.smoo
        ) : select2(lc_en)
    )
);

preAmpWet = _ * input_vol : m1
          : s1_block
          : ( _ <: _, s2_block : select2(preamp_stages > 0) ) 
          : ( _ <: _, s3_block : select2(preamp_stages > 1) )
          : interstage_dc : *(preamp_out * preamp_level_comp * gk_comp)
          : post_eq_group;

// -----------------------------------------------------------------------------
// Bypass
// -----------------------------------------------------------------------------
bypass_preamp   = hgroup("2 Preamp", checkbox("Bypass"));
bypass_preamp_f = float(bypass_preamp) : si.smoo;
bypass_gain     = hgroup("2 Preamp", hslider("Bypass Gain[style:knob]", 8.0, 0.1, 20.0, 0.01)) : si.smoo;

preAmpDry = _ * bypass_gain;

preAmp = _ <: preAmpDry, preAmpWet
       : (*(bypass_preamp_f), *(1.0 - bypass_preamp_f))
       : +;
