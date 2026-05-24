import("stdfaust.lib");

// =====================================================================
// IFCglobalPreset.dsp — Global Amp Model Selector
// =====================================================================
// This file is imported by all amp-sim modules. Faust's import guards
// prevent duplicate definitions when multiple modules import it.
//
// ampModel drives the entire signal chain:
//   0 = Lab (manual) — all module-specific knobs are active
//   1 = Mesa Rectifier — fat, compressed, cold hi-gain
//   2 = Fender Deluxe — clean/breakup, warm, headroom
//   3 = Marshall JCM 800 — crunch, midrange, moderate compression
//   4 = Soldano SLO-100 — smooth hi-gain, bloom, rich harmonics
//   5 = Vox AC30 — chimey, brilliant, Class A feel
// =====================================================================

declare name      "IFCglobalPreset";
declare author    "Michel Buffa & Jerome Lebrun";
declare version   "1.0";
declare license   "LGPL";

// ---- Global selector ------------------------------------------------
ampModel = hgroup("[00] Amp Model",
    nentry("[00] Amp[style:menu{'Lab (Manual)':0;'Mesa Rectifier':1;'Fender Deluxe':2;'Marshall JCM 800':3;'Soldano SLO-100':4;'Vox AC30':5}]",
    1, 0, 5, 1)) : int;

// ---- Helper: choose value by amp preset -----------------------------
// Index 0 = Lab/manual (uses the first argument, typically a knob value)
// Indices 1-5 = preset values for Mesa, Fender, JCM, Soldano, Vox
choose5ap(lab, mesa, fender, jcm, soldano, vox) =
    ba.selectn(6, ampModel, (lab, mesa, fender, jcm, soldano, vox));

// ---- Convenience flags ----------------------------------------------
isLab = ampModel == 0;
