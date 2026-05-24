import("stdfaust.lib");

import("IFCglobalPreset.dsp");
import("IFCtonestackMulti.dsp");
// comment this line and uncomment next to switch from one preamp to another
//import("IFCpreampGridCurrentCathodeBiasOptionnalDynStage.dsp");
//import("IFCpreampGuitarix.dsp");
import("IFCpreampGuitarix_multivoice.dsp"); // version corrigée pour multi étages, gain etc.
import("IFCpowerAmp.dsp");
import("IFCreverbMulti.dsp");
import("IFCcabSim.dsp");


declare name "guitar tube amp sim 100% FAUST";
declare author "Michel Buffa & Jerome Lebrun";
declare version "0.2";
declare category "Amp";

// This is the main chain of stages for a complete 100% FAUST amp sim using grey box approach
// Note: two different preamps can be tried (just change imports at beginning of this file)
// reverb, tonestack, power amp, cab sim are customizable. The preamp is the most important 
// part of the amp. The guitarix preamp is super good for clean sounds (aka "fender sounds")
// and uses the tube model from the guitarix project that has been modified not to use 
// look-up tables in an external file anymore.
// The other preamp is a custom one that proposes lots of customization and presets for 
// different guitar amps, ADAA anti-aliasing, etc.

// Output gain at the end of the chain, before the cabinet simulation just an output volume
outputLevel = hgroup("Output[stratus:4]", hslider("Level[style:knob]", 1, 0.0, 1.0, 0.01)) : si.smoo;
output_gain = ba.db2linear((outputLevel - 0.5) * 24 + (-30.00));  // norm: -30.0dB

// ---- Post-preamp EQ driven by ampModel ----
// In Lab mode (ampModel=0): manual knobs are active.
// In preset mode (ampModel>0): calibrated values per amp model.
postEQ = fi.low_shelf(pLS_g, pLS_f) : fi.peak_eq_cq(pMid_g, pMid_f, pMid_q) : fi.high_shelf(pHS_g, pHS_f)
with {
    // Lab knobs (used when ampModel=0)
    pLS_g_knob  = hgroup("3 Post EQ", hslider("[1]Low Shelf dB[style:knob]",   8.5, -12.0, 18.0, 0.1))  : si.smoo;
    pLS_f_knob  = hgroup("3 Post EQ", hslider("[2]Low Shelf Hz[style:knob]",   200.0, 80.0, 400.0, 1.0)) : si.smoo;
    pMid_g_knob = hgroup("3 Post EQ", hslider("[3]Mid Peak dB[style:knob]",    0.0, -12.0, 18.0, 0.1))   : si.smoo;
    pMid_f_knob = hgroup("3 Post EQ", hslider("[4]Mid Peak Hz[style:knob]",    1000.0, 200.0, 5000.0, 1.0)) : si.smoo;
    pMid_q_knob = hgroup("3 Post EQ", hslider("[5]Mid Peak Q[style:knob]",     1.0, 0.3, 5.0, 0.01))     : si.smoo;
    pHS_g_knob  = hgroup("3 Post EQ", hslider("[6]Hi Shelf dB[style:knob]",    -10.0, -20.0, 6.0, 0.1))  : si.smoo;
    pHS_f_knob  = hgroup("3 Post EQ", hslider("[7]Hi Shelf Hz[style:knob]",    4000.0, 2000.0, 10000.0, 10.0)) : si.smoo;

    // Preset values:       Lab knob       Mesa    Fender  JCM800  Soldano Vox
    pLS_g  = choose5ap(pLS_g_knob,         10.0,   6.0,    8.0,    9.0,    5.0)  : si.smoo;
    pLS_f  = choose5ap(pLS_f_knob,         180.0,  200.0,  190.0,  180.0,  220.0) : si.smoo;
    pMid_g = choose5ap(pMid_g_knob,        -3.0,   2.0,    5.0,    3.0,    4.0)   : si.smoo;
    pMid_f = choose5ap(pMid_f_knob,        800.0,  1200.0, 1000.0, 900.0,  1800.0) : si.smoo;
    pMid_q = choose5ap(pMid_q_knob,        1.5,    1.0,    1.2,    1.3,    0.8)   : si.smoo;
    pHS_g  = choose5ap(pHS_g_knob,         -6.0,   -8.0,   -7.0,   -5.0,   -4.0)  : si.smoo;
    pHS_f  = choose5ap(pHS_f_knob,         6000.0, 4500.0, 4500.0, 5500.0, 5000.0) : si.smoo;
};


monoChain =
        fi.dcblocker
        : (fi.highpass(1, 23.0) : fi.peak_eq_cq(2.4, 1222.4, 0.26))
        : preAmp
        : postEQ
        : tonestack_stage
        : powerAmp_stage
        : fi.dcblocker
        : *(output_gain)
        : reverbMulti
        :> cabSim:>_;


process = monoChain <: _, _;