// ============================================================
// tonestack.dsp
// Guitar tonestack emulation with selectable amplifier model
// Bass / Middle / Treble controls (left to right)
// Models from tonestacks.lib (D.T. Yeh)
// ============================================================

// ---- component-level aliases -----------------------------------------------
import("stdfaust.lib");

ma = library("maths.lib");
fi = library("filters.lib");
ba = library("basics.lib");


declare name      "IFCtonestack";
declare author    "Michel Buffa / based on Guitarix tonestacks.lib";
declare version   "1.0";
declare license   "LGPL";


// NOTE: this is a "multi tonestack that proposes several presets corresponding
// to the tonestack of several amp brands. It should be improved by using the new
// on demand feature of FAUST, so that the cpu usage will correspond only to the
// selected tonestack. This is a first step in that direction.


// ---- resistor / capacitor unit helpers -------------------------------------
k = *(1e3);
M = *(1e6);
nF = *(1e-9);
pF = *(1e-12);

// ---- core tonestack IIR transfer function ----------------------------------
// signature: tonestack(C1,C2,C3,R1,R2,R3,R4, treble, middle, bass)
tonestack(C1,C2,C3,R1,R2,R3,R4,t,m,L) =
    1/A0*fi.iir((B0,B1,B2,B3),(A1/A0,A2/A0,A3/A0))
    with {
    l = L : (_-1)*3.4 : exp;

    b1 = t*C1*R1 + m*C3*R3 + l*(C1*R2 + C2*R2) + (C1*R3 + C2*R3);
    b2 = t*(C1*C2*R1*R4 + C1*C3*R1*R4) - m*m*(C1*C3*R3*R3 + C2*C3*R3*R3)
         + m*(C1*C3*R1*R3 + C1*C3*R3*R3 + C2*C3*R3*R3)
         + l*(C1*C2*R1*R2 + C1*C2*R2*R4 + C1*C3*R2*R4)
         + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
         + (C1*C2*R1*R3 + C1*C2*R3*R4 + C1*C3*R3*R4);
    b3 = l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
         - m*m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
         + m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
         + t*C1*C2*C3*R1*R3*R4 - t*m*C1*C2*C3*R1*R3*R4
         + t*l*C1*C2*C3*R1*R2*R4;

    a0 = 1;
    a1 = (C1*R1 + C1*R3 + C2*R3 + C2*R4 + C3*R4)
         + m*C3*R3 + l*(C1*R2 + C2*R2);
    a2 = m*(C1*C3*R1*R3 - C2*C3*R3*R4 + C1*C3*R3*R3 + C2*C3*R3*R3)
         + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
         - m*m*(C1*C3*R3*R3 + C2*C3*R3*R3)
         + l*(C1*C2*R2*R4 + C1*C2*R1*R2 + C1*C3*R2*R4 + C2*C3*R2*R4)
         + (C1*C2*R1*R4 + C1*C3*R1*R4 + C1*C2*R3*R4 + C1*C2*R1*R3 + C1*C3*R3*R4 + C2*C3*R3*R4);
    a3 = l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
         - m*m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
         + m*(C1*C2*C3*R3*R3*R4 + C1*C2*C3*R1*R3*R3 - C1*C2*C3*R1*R3*R4)
         + l*C1*C2*C3*R1*R2*R4
         + C1*C2*C3*R1*R3*R4;

    c  = 2*float(ma.SR);
    B0 = -b1*c - b2*pow(c,2) - b3*pow(c,3);
    B1 = -b1*c + b2*pow(c,2) + 3*b3*pow(c,3);
    B2 =  b1*c + b2*pow(c,2) - 3*b3*pow(c,3);
    B3 =  b1*c - b2*pow(c,2) + b3*pow(c,3);
    A0 = -a0 - a1*c - a2*pow(c,2) - a3*pow(c,3);
    A1 = -3*a0 - a1*c + a2*pow(c,2) + 3*a3*pow(c,3);
    A2 = -3*a0 + a1*c + a2*pow(c,2) - 3*a3*pow(c,3);
    A3 = -a0 + a1*c - a2*pow(c,2) + a3*pow(c,3);
};

// ---- per-model convenience wrappers ----------------------------------------
// All wrappers: (treble, middle, bass) parameter order matches tonestack()
mesa(T,M_,L)          = tonestack(250:pF,100:nF, 47:nF, 250:k, 250:k, 25:k, 100:k, T,M_,L);
jcm800(T,M_,L)        = tonestack(470:pF, 22:nF, 22:nF, 220:k,   1:M, 22:k, 33:k,  T,M_,L);
ac30(T,M_,L)          = tonestack( 50:pF, 22:nF, 22:nF,   1:M,   1:M, 10:k, 100:k, T,M_,L);
fender_deville(T,M_,L)= tonestack(250:pF,100:nF, 22:nF, 250:k, 250:k, 25:k, 130:k, T,M_,L);

// ---- number of models (must match the list below) --------------------------
NMODELS = 4;

// ---- UI controls -----------------------------------------------------------
// vgroup "Tonestack"
//   hgroup "EQ"  : 3 knobs  (Bass | Middle | Treble)
//   hgroup "Type": label + menu selector
bass   = hgroup("Tonestack", hgroup("EQ", vslider("Bass[1][style:knob]",   0.5, 0, 1, 0.01)));
middle = hgroup("Tonestack", hgroup("EQ", vslider("Middle[2][style:knob]", 0.3, 0, 1, 0.01)));
treble = hgroup("Tonestack", hgroup("EQ", vslider("Treble[3][style:knob]", 0.75, 0, 1, 0.01)));
model  = hgroup("Tonestack", hgroup("tonestack type",
           nentry("Model[style:menu{'Mesa Boogie':0;'JCM800':1;'AC30':2;'Fender Hot Rod':3}]",
                  0, 0, 3, 1))) : int;

// ---- per-model makeup gain to compensate passive attenuation ----------------
// Passive tonestacks inherently lose signal (typically -15 to -25 dB depending
// on component values and knob settings). These makeup gains bring each model
// back to approximately unity gain at "noon" settings (bass=0.5, mid=0.3, treble=0.75).
// Values are in linear gain (dB equivalent shown in comments).
//                                Mesa     JCM800   AC30     Fender Deville
//                                ~+20dB   ~+18dB   ~+22dB   ~+16dB
makeupGainTable = 10.0, 8.0, 12.5, 6.3;
makeupGain = makeupGainTable : ba.selectn(NMODELS, model);

// ---- model selector using ba.selectn ---------------------------------------
// ba.selectn(N, k) : (sig0, sig1, ..., sigN-1) -> sig_k
selectedModel =
    _ <:
        mesa(treble, middle, bass),
        jcm800(treble, middle, bass),
        ac30(treble, middle, bass),
        fender_deville(treble, middle, bass)
    : ba.selectn(NMODELS, model)
    : *(makeupGain);

// ---- process ---------------------------------------------------------------
selectedModel1 = hgroup("Tonestack[stratus:3]", ba.bypass_fade(ma.SR/10, checkbox("bypass"), selectedModel));

tonestack_stage = selectedModel1;

