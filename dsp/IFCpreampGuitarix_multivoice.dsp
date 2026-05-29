import("stdfaust.lib");
import("IFCguitarixTubes.dsp");

// IFC configurable guitar preamp — v6 Simplified Amp Presets
// ============================================================
// A single "Amp Model" menu controls the preamp + integrated post-EQ.
// - Lab mode (0): all knobs active for manual experimentation
// - Preset modes (1-5): calibrated values, only Gain/Output/Trim adjustable
//
// The tonestack, power amp, cab remain independently controlled
// by their own UI (no duplicate menu issues).
//
// Optional features (checkbox):
//   - Cathode Bias Sag: dynamic compression on loud signals
//   - Bias Offset: asymmetric clipping for H2 harmonics

preampMB = *(inputDrive)
    : m_in
    : Stage1 : m_s1
    : ((_ <: _, (pre12 : Stage2)) : select2(useStage2)) : m_12
    : ((_ <: _, (pre23 : Stage3)) : select2(useStage3)) : m_23
    : ((_ <: _, (pre34 : Stage4)) : select2(useStage4)) : m_34
    : ((_ <: _, (pre45 : Stage5)) : select2(useStage5)) : m_45
    : sag_block
    : *(outputGain * outComp)
    : m_out
    : postEQ
with {
    // ---------- small utilities ----------
    db2lin(db) = pow(10.0, db/20.0);
    pot01(x) = pow(x/10.0, 2.2);
    rangePot(x, lo, hi) = lo + (hi-lo) * pot01(x);

    // ---------- Amp Model selector (ONLY defined here) ----------
    // 0=Lab, 1=Mesa Rectifier, 2=Fender Deluxe, 3=JCM 800, 4=Soldano, 5=Vox AC30
    ampModel = hgroup("Preamp/[00] Amp Model",
        nentry("Amp[style:menu{'Lab (Manual)':0;'Mesa Rectifier':1;'Fender Deluxe':2;'Marshall JCM 800':3;'Soldano SLO-100':4;'Vox AC30':5}]",
        1, 0, 5, 1)) : int;

    isLab = ampModel == 0;
    choose5ap(lab, mesa, fender, jcm, soldano, vox) =
        ba.selectn(6, ampModel, (lab, mesa, fender, jcm, soldano, vox));

    // ---------- main controls ----------
    // These knobs are ALWAYS active (like a real amp front panel)
    gainKnob     = hgroup("Preamp/[01] Controls", hslider("[00] Gain[style:knob]", 5.0, 0.0, 10.0, 0.1)) : si.smooth(0.999);
    inputTrimDB  = hgroup("Preamp/[01] Controls", hslider("[01] Input Trim[unit:dB][style:knob]", 0.0, -18.0, 18.0, 0.1)) : si.smooth(0.999);
    outputKnob   = hgroup("Preamp/[01] Controls", hslider("[02] Output Level[style:knob]", 5.0, 0.0, 10.0, 0.1)) : si.smooth(0.999);

    // ---------- Lab-only controls ----------
    // These menus/knobs are active in Lab mode. In preset mode, values are overridden.
    nbStages_lab = hgroup("Preamp/[02] Lab Controls", nentry("[10] NbStages[style:menu{'1 stage':1;'2 stages':2;'3 stages':3;'4 stages':4;'5 stages':5}]", 5, 1, 5, 1));
    gpMenu       = hgroup("Preamp/[02] Lab Controls", nentry("[11] GainPlacement[style:menu{'between 1-2':1;'between 2-3':2;'between 3-4':3;'All stages (mild)':4}]", 4, 1, 4, 1));
    hpManual     = hgroup("Preamp/[02] Lab Controls", hslider("[12] Tightness HP[unit:Hz][style:knob]", 120.0, 20.0, 320.0, 1.0)) : si.smooth(0.999);
    lpManual     = hgroup("Preamp/[02] Lab Controls", hslider("[13] Brightness LP[unit:Hz][style:knob]", 8000.0, 2500.0, 14000.0, 10.0)) : si.smooth(0.999);
    interTrimDB  = hgroup("Preamp/[02] Lab Controls", hslider("[14] Interstage Trim[unit:dB][style:knob]", 0.0, -18.0, 12.0, 0.1)) : si.smooth(0.999);

    // ---------- preset-driven parameters ----------
    //                           Lab             Mesa  Fender JCM    Soldano Vox
    nbStages = int(choose5ap(nbStages_lab,       5,    3,     4,     5,      3));
    gainPlacement = int(choose5ap(gpMenu,        4,    1,     1,     4,      1));

    // Gain ranges — calibrated per amp character
    // Fender: clean at 5, light breakup at 8-9, edge-of-breakup at 10
    // Mesa: crunch at 3, defined hi-gain at 5, metal at 7, brutal at 10
    mainGain = rangePot(gainKnob,
        choose5ap(1.00,                          4.00,  0.40,  2.00,  4.00,   0.50),
        choose5ap(16.00,                         22.00, 2.50,  10.00, 20.00,  3.50));

    // HP/LP interstage filters
    hp = choose5ap(hpManual,                     90.0,  30.0,  70.0,  85.0,   50.0) : si.smooth(0.999);
    lp = choose5ap(lpManual,                     9000.0, 12000.0, 10000.0, 9000.0, 11000.0) : si.smooth(0.999);

    // Interstage attenuation — DECREASING through stages for definition
    // Key insight: stages 4-5 must NOT be overdriven (creates blurry sound)
    // Fender/Vox: near-unity (clean amps, gain via strategic placement only)
    // Mesa/Soldano: steep decrease (stages 1-2 do the work, 4-5 shape tone)
    //                           Lab             Mesa  Fender JCM    Soldano Vox
    att12Default = choose5ap(0.80,               0.85, 0.92,  0.82,  0.82,   0.90);
    att23Default = choose5ap(0.70,               0.60, 0.88,  0.68,  0.58,   0.85);
    att34Default = choose5ap(0.60,               0.38, 0.85,  0.55,  0.36,   0.85);
    att45Default = choose5ap(0.55,               0.25, 0.80,  0.45,  0.24,   0.80);

    interTrim = select2(isLab, 1.0, db2lin(interTrimDB));

    // Input drive multiplier (before stage 1)
    // Mesa: 1.8x — warm input, stage 1 clips on strong transients
    // Fender: 0.8x — REDUCE input to keep clean headroom
    // Vox: 0.9x — slightly below unity for chimey clean
    inputDrive = db2lin(inputTrimDB) *
        choose5ap(1.00,                          1.80, 0.80,  1.30,  1.70,   0.90);

    outputGain = rangePot(outputKnob, 0.05, 3.0);

    // ---------- independent topology ----------
    useStage2 = nbStages >= 2;
    useStage3 = nbStages >= 3;
    useStage4 = nbStages >= 4;
    useStage5 = nbStages >= 5;

    // ---------- gain distribution ----------
    mildGain = sqrt(max(0.001, mainGain));

    // Strategic mode: gain injected at ONE point
    g12Strategic = select2(gainPlacement == 1, att12Default, mainGain);
    g23Strategic = select2(gainPlacement == 2, att23Default, mainGain);
    g34Strategic = select2(gainPlacement == 3, att34Default, mainGain);

    // "All stages" mode (4): sqrt(gain) distributed before every stage
    // This is KEY for hi-gain — each stage clips independently
    g12 = select2(gainPlacement == 4, g12Strategic, att12Default * mildGain) * interTrim;
    g23 = select2(gainPlacement == 4, g23Strategic, att23Default * mildGain) * interTrim;
    g34 = select2(gainPlacement == 4, g34Strategic, att34Default * mildGain) * interTrim;
    g45 = select2(gainPlacement == 4, att45Default, att45Default * mildGain) * interTrim;

    // Output compensation — calibrated per preset
    //                            Lab             Mesa  Fender JCM    Soldano Vox
    stageComp = choose5ap(ba.selectn(5, nbStages-1, (1.00, 0.86, 0.68, 0.52, 0.40)),
                                                 0.45, 0.92,  0.65,  0.42,   0.90);
    profileComp = choose5ap(1.0,                 0.60, 0.95,  0.75,  0.55,   0.92);
    outComp = stageComp * profileComp;

    // ---------- cathode sag (optional) ----------
    sagEnabled  = hgroup("Preamp/[03] Dynamics", checkbox("[40] Cathode Sag"));
    sagTau_knob = hgroup("Preamp/[03] Dynamics", hslider("[41] Sag Time (ms)[style:knob]", 30.0, 5.0, 200.0, 1.0)) : si.smooth(0.999);
    sagAmt_knob = hgroup("Preamp/[03] Dynamics", hslider("[42] Sag Amount[style:knob]", 0.15, 0.0, 0.5, 0.01)) : si.smooth(0.999);

    sagTau = choose5ap(sagTau_knob,              18.0, 50.0,  35.0,  25.0,   55.0) : si.smooth(0.999);
    sagAmt = choose5ap(sagAmt_knob,              0.22, 0.08,  0.15,  0.20,   0.18) : si.smooth(0.999);

    sag_alpha = exp(-1.0 / max(1.0, sagTau * 0.001 * ma.SR));
    sag_core(x) = x * max(0.1, 1.0 - sagAmt * (abs(x) : si.smooth(sag_alpha)));
    sag_block = _ <: _, sag_core : select2(sagEnabled);

    // ---------- bias offset (optional) ----------
    biasEnabled  = hgroup("Preamp/[03] Dynamics", checkbox("[43] Bias Offset (H2)"));
    biasOff_knob = hgroup("Preamp/[03] Dynamics", hslider("[44] Bias Amount[style:knob]", -0.08, -0.3, 0.1, 0.001)) : si.smooth(0.999);

    biasOff = choose5ap(biasOff_knob,            -0.15, -0.03, -0.08, -0.12,  -0.05) : si.smooth(0.999);
    biasAdd = biasOff * float(biasEnabled);

    // ---------- interstage filters ----------
    filt12 = fi.highpass(1, max(20.0, hp*0.6)) : fi.lowpass(1, lp);
    filt23 = fi.highpass(1, hp) : fi.lowpass(1, lp);
    filt34 = fi.highpass(1, hp) : fi.lowpass(1, max(2500.0, lp*0.92));
    filt45 = fi.highpass(1, hp) : fi.lowpass(1, max(2500.0, lp*0.85));

    pre12 = *(g12) : filt12;
    pre23 = *(g23) : filt23;
    pre34 = *(g34) : filt34;
    pre45 = *(g45) : filt45;

    // ---------- tube selection ----------
    tube_menu1 = hgroup("Preamp/[04] Tube choices", nentry("[30] Stage 1 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu2 = hgroup("Preamp/[04] Tube choices", nentry("[31] Stage 2 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu3 = hgroup("Preamp/[04] Tube choices", nentry("[32] Stage 3 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu4 = hgroup("Preamp/[04] Tube choices", nentry("[33] Stage 4 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu5 = hgroup("Preamp/[04] Tube choices", nentry("[34] Stage 5 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));

    Stage1 = +(biasAdd) : (tube_menu1, (_ <: T1_12AX7, T1_12AT7, T1_12AU7, T1_6V6, T1_6DJ8, T1_6C16)) : ba.selectn(6);
    Stage2 = +(biasAdd) : (tube_menu2, (_ <: T2_12AX7, T2_12AT7, T2_12AU7, T2_6V6, T2_6DJ8, T2_6C16)) : ba.selectn(6);
    Stage3 = +(biasAdd) : (tube_menu3, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);
    Stage4 = +(biasAdd) : (tube_menu4, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);
    Stage5 = +(biasAdd) : (tube_menu5, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);

    // ---------- integrated post-EQ (driven by ampModel, no duplicate menu) ----------
    postEQ = fi.low_shelf(pLS_g, pLS_f) : fi.peak_eq_cq(pMid_g, pMid_f, pMid_q) : fi.high_shelf(pHS_g, pHS_f)
    with {
        // Lab knobs
        pLS_g_knob  = hgroup("Preamp/[05] Post EQ", hslider("[1]Low Shelf dB[style:knob]",   8.5, -12.0, 18.0, 0.1))  : si.smoo;
        pLS_f_knob  = hgroup("Preamp/[05] Post EQ", hslider("[2]Low Shelf Hz[style:knob]",   200.0, 80.0, 400.0, 1.0)) : si.smoo;
        pMid_g_knob = hgroup("Preamp/[05] Post EQ", hslider("[3]Mid Peak dB[style:knob]",    0.0, -15.0, 15.0, 0.1))   : si.smoo;
        pMid_f_knob = hgroup("Preamp/[05] Post EQ", hslider("[4]Mid Peak Hz[style:knob]",    1000.0, 200.0, 5000.0, 1.0)) : si.smoo;
        pMid_q_knob = hgroup("Preamp/[05] Post EQ", hslider("[5]Mid Peak Q[style:knob]",     1.0, 0.3, 5.0, 0.01))     : si.smoo;
        pHS_g_knob  = hgroup("Preamp/[05] Post EQ", hslider("[6]Hi Shelf dB[style:knob]",    -10.0, -20.0, 6.0, 0.1))  : si.smoo;
        pHS_f_knob  = hgroup("Preamp/[05] Post EQ", hslider("[7]Hi Shelf Hz[style:knob]",    4000.0, 2000.0, 10000.0, 10.0)) : si.smoo;

        //                        Lab knob        Mesa    Fender  JCM800  Soldano Vox
        pLS_g  = choose5ap(pLS_g_knob,            8.0,    3.0,    7.0,    8.0,    2.5)   : si.smoo;
        pLS_f  = choose5ap(pLS_f_knob,            180.0,  200.0,  190.0,  180.0,  220.0) : si.smoo;
        pMid_g = choose5ap(pMid_g_knob,           -2.5,   0.0,    4.0,    1.0,    3.0)   : si.smoo;
        pMid_f = choose5ap(pMid_f_knob,           800.0,  1200.0, 1000.0, 900.0,  1800.0): si.smoo;
        pMid_q = choose5ap(pMid_q_knob,           1.5,    1.0,    1.2,    1.3,    0.8)   : si.smoo;
        pHS_g  = choose5ap(pHS_g_knob,            -6.0,   -4.0,   -6.0,   -5.0,   -2.0)  : si.smoo;
        pHS_f  = choose5ap(pHS_f_knob,            6000.0, 5000.0, 4500.0, 5500.0, 5500.0) : si.smoo;
    };

    // ---------- meters ----------
    pk_decay = exp(-1.0 / (0.5 * ma.SR));
    p_hold = max ~ (*(pk_decay));

    meter(x, m) = x : attach(_, abs(x) : p_hold : ba.linear2db : m);

    mtr_in = hbargraph("Preamp/[90] Meters/[0] Input[unit:dB]", -60, 10);
    mtr_s1 = hbargraph("Preamp/[90] Meters/[1] After Stage1[unit:dB]", -60, 10);
    mtr_12 = hbargraph("Preamp/[90] Meters/[2] Inter 1-2[unit:dB]", -60, 10);
    mtr_23 = hbargraph("Preamp/[90] Meters/[3] Inter 2-3[unit:dB]", -60, 10);
    mtr_34 = hbargraph("Preamp/[90] Meters/[4] Inter 3-4[unit:dB]", -60, 10);
    mtr_45 = hbargraph("Preamp/[90] Meters/[5] Inter 4-5[unit:dB]", -60, 10);
    mtr_out = hbargraph("Preamp/[90] Meters/[6] Output[unit:dB]", -60, 10);

    m_in(x) = meter(x, mtr_in);
    m_s1(x) = meter(x, mtr_s1);
    m_12(x) = meter(x, mtr_12);
    m_23(x) = meter(x, mtr_23);
    m_34(x) = meter(x, mtr_34);
    m_45(x) = meter(x, mtr_45);
    m_out(x) = meter(x, mtr_out);
};

// Root UI group
preampMono = vgroup("Preamp v6", ba.bypass_fade(ma.SR/10, checkbox("[99] bypass"), preampMB));
preAmp = preampMono;
//process = preAmp;
