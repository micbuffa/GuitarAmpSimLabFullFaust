import("stdfaust.lib");

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
declare version "0.3";
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

// Post-EQ is now integrated inside the preamp module (driven by ampModel preset).
// Tonestack, power amp, cab, reverb are independently controlled.
monoChain =
        fi.dcblocker
        : (fi.highpass(1, 23.0) : fi.peak_eq_cq(2.4, 1222.4, 0.26))
        : preAmp
        : tonestack_stage
        : powerAmp_stage
        : fi.dcblocker
        : *(output_gain)
        : reverbMulti
        :> cabSim:>_;


process = monoChain <: _, _;