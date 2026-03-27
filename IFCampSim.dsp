import("stdfaust.lib");

import("IFCtonestackMulti.dsp");
// comment this line and uncomment next to switch from one preamp to another
import("IFCpreampGridCurrentCathodeBiasOptionnalDynStage.dsp");
//import("IFCpreampGuitarix.dsp");
import("IFCpowerAmp.dsp");
import("IFCreverbMulti.dsp");
import("IFCcabSim.dsp");


declare name "guitar tube amp sim 100% FAUST";
declare author "Michel Buffa & Jerome Lebrun";
declare version "0.1";
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
outputLevel = hgroup("Output[stratus:4]", hslider("Level[style:knob]", 0.5, 0.0, 1.0, 0.01)) : si.smoo;
output_gain = ba.db2linear((outputLevel - 0.5) * 24 + (-30.00));  // norm: -30.0dB

// ── Process chain ───────────────────────────────────────
// Note about the first set of filters before the preamp:
// input filter of the preamp
//  - (fi.highpass(1, 23.0) 
//      Removes DC and useless sub-bass. 
//      Prevents DC or very low frequencies from driving 
//      the saturation asymmetrically. Equivalent to a coupling capacitor 
//      at the input.
//  - fi.peak_eq_cq(2.4, 1222.4, 0.26) : Gentle and wide boost 
//      (+2.4 dB, Q very low = almost a whole octave) centered around 1.2 kHz. 
//      Slightly accentuates the presence and "pick attack" of guitars, 
//      making the saturation more biting in the mids.
//  - Audible effect : the sound enters the preamp with slightly more 
//    energy in the mids → more "biting" and clear saturation, 
//    less flabby in the low-mids.
// 
// Note about the second set of filters after the preamp 
// (Coupling EQ after the preamp and before the tonestack)
//  - fi.low_shelf(8.52, 200) : 
//    Strong bass boost. Recovers low-frequency energy "swallowed" by 
//    preamp saturation (saturation compresses transients and 
//    flattens the lows). Simulates the response of the output transformer and 
//    power supply filtering in a real amp.
//  - fi.peak_eq_cq(13.63, 3943.9, 2.35) : 
//    Strong presence boost in the 3–5 kHz range. This is the "amp's bite" zone — 
//    the clarity, the pick attack, what makes a distortion "breathe". 
//    Typical of the coloration of a Marshall or Mesa from a tube stage.
//  - fi.high_shelf(-10.11, 4000) : 
//    Strongly cuts the treble to prevent the distortion from spitting out 
//    harsh and aggressive high frequencies. Together with the previous 
//    peak-eq, this creates a "presence bump" followed by a drop → this is 
//    exactly the typical response curve of a tube amp with its output 
//    transformer.
//
//    Audible effect : the post-saturation sound gains body in the lows, 
//    "bites" in the upper mids, and the high-frequency aggressiveness of 
//    the distortion is tamed. The overall result resembles the natural 
//    coloration of a tube amp even before the tonestack comes into play.
//
//    In summary : these two blocks are "glue" timbre correctors. 
//    They compensate for the spectral effects of saturation (loss of bass, 
//    excess treble) and inject the "tube amp" color into the chain, in the 
//    way that passive components (transformers, cathode capacitors) do in a 
//    real circuit.


monoChain =
        fi.dcblocker
        : (fi.highpass(1, 23.0) : fi.peak_eq_cq(2.4, 1222.4, 0.26))
        : preAmp
        : (fi.low_shelf(8.52, 200) : fi.peak_eq_cq(13.63, 3943.9, 2.35) : fi.high_shelf(-10.11, 4000))
        : tonestack_stage
        : powerAmp_stage
        : fi.dcblocker
        : *(output_gain)
        : reverbMulti
        :> cabSimMB:>_;


process = monoChain <: _, _;