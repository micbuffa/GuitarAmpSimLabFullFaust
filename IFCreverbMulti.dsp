import("stdfaust.lib");

re = library("reverbs.lib");
si = library("signals.lib");

declare name      "IFCreverbMulti";
declare author    "Michel Buffa";
declare version   "1.0";
declare license   "LGPL";

// This is a reverb module that proposes several presets corresponding
// to different types of reverbs (Freeverb, Spring, Zita). It should be improved by using 
// the new on demand feature of FAUST, so that the cpu usage will correspond only to the
// selected reverb.

// ------------------------------------------------------------
// UI finale
// ------------------------------------------------------------
mixReverb = hslider("Reverb/Mix [style:knob]", 0.18, 0.0, 1.0, 0.001) : si.smoo;

// Menu : 0 = Freeverb, 1 = Spring, 2 = Zita
algo = int(nentry("Reverb/Type [style:menu{'Freeverb':0;'Spring':1;'Zita':2}]", 2, 0, 2, 1));

// ------------------------------------------------------------
// Defaults "guitare"
// ------------------------------------------------------------

// Freeverb : discrete, rather simple room/plate
wetFree =
    _ <: _,_
    : re.stereo_freeverb(0.72, 0.50, 0.35, 23)
    : +
    : *(0.5 * mixReverb);

// Spring : more typical of guitar amps
wetSpring =
    re.springreverb(0.55, 0.35, 0.45, 0.42, 1)
    : *(mixReverb);

// Zita : cleaner / modern / studio
wetZita =
    _ <: _,_
    : re.zita_rev1_stereo(28.0, 300.0, 3800.0, 2.2, 1.4, 48000.0)
    : +
    : *(0.5 * mixReverb);

// 3-way selector via nested select2
// algo = 0 -> wetFree
// algo = 1 -> wetSpring
// algo = 2 -> wetZita
wet =
    select2(algo == 0,
        select2(algo == 1, wetZita, wetSpring),
        wetFree
    );

// ------------------------------------------------------------
// Mono in -> mono out
// ------------------------------------------------------------
reverbMulti = _ <: (*(1.0 - mixReverb)), wet : +;

//process = reverbMulti;