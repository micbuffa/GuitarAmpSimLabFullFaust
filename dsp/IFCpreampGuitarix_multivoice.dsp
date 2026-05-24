import("stdfaust.lib");
import("IFCguitarixTubes.dsp");
import("IFCglobalPreset.dsp");

// IFC configurable guitar preamp — v5 Global Amp Preset System
// =============================================================
// When ampModel = 0 (Lab): all existing menus (ToneProfile, Voicing,
//   NbStages, GainPlacement) work exactly as before.
// When ampModel = 1-5: these menus are overridden by calibrated values
//   for each amp model (Mesa Rectifier, Fender Deluxe, JCM 800,
//   Soldano SLO-100, Vox AC30).
//
// New optional features (checkbox):
//   - Cathode Bias Sag: dynamic gain reduction on loud signals
//   - Bias Offset: asymmetric clipping for H2 harmonics (tube warmth)

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
with {
    // ---------- small utilities ----------
    db2lin(db) = pow(10.0, db/20.0);
    pot01(x) = pow(x/10.0, 2.2);
    rangePot(x, lo, hi) = lo + (hi-lo) * pot01(x);

    choose6(a,b,c,d,e,f) = ba.selectn(6, toneProfile, (a,b,c,d,e,f));
    choose7v(a,b,c,d,e,f,g) = ba.selectn(7, voicing, (a,b,c,d,e,f,g));

    // ---------- main controls ----------
    // These menus are used in Lab mode (ampModel=0).
    // When ampModel>0, their values are overridden below via choose5ap.
    toneProfile = hgroup("Preamp Guitarix Configurable/[10] Menus", nentry("[20] ToneProfile[style:menu{'Jazz':0;'Clean':1;'Crunch':2;'Crunch++':3;'Hi Gain':4;'Hi Gain++':5}]", 4, 0, 5, 1));

    nbStages_lab = hgroup("Preamp Guitarix Configurable/[10] Menus", nentry("[21] NbStages[style:menu{'1 stage':1;'2 stages':2;'3 stages':3;'4 stages':4;'5 stages':5}]", 5, 1, 5, 1));
    // ampModel overrides stage count:
    //                              Lab       Mesa Fender JCM  Soldano Vox
    nbStages = int(choose5ap(nbStages_lab,    5,   3,     4,   5,      3));

    gainPlacementMenu = hgroup("Preamp Guitarix Configurable/[10] Menus", nentry("[22] GainPlacement[style:menu{'Profile default':0;'between 1 and 2':1;'between 2 and 3':2;'between 3 and 4':3;'All mild':4}]", 0, 0, 4, 1));

    voicingMenu = hgroup("Preamp Guitarix Configurable/[10] Menus", nentry("[23] Voicing[style:menu{'Profile default':0;'Warm Jazz':1;'Balanced Clean':2;'Vintage Crunch':3;'Hot Crunch':4;'Recto Vintage':5;'Recto Modern':6}]", 0, 0, 6, 1));

    usePresetVoicing = hgroup("Preamp Guitarix Configurable/[10] Menus", checkbox("[24] Use preset voicing"));

    gainKnob = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[00] Main Gain[style:knob]", 5.0, 0.0, 10.0, 0.1)) : si.smooth(0.999);
    inputTrimDB = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[01] Input Trim[unit:dB][style:knob]", 0.0, -18.0, 18.0, 0.1)) : si.smooth(0.999);
    outputKnob = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[02] Output Level[style:knob]", 5.0, 0.0, 10.0, 0.1)) : si.smooth(0.999);

    hpManual = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[03] Manual Tightness HP[unit:Hz][style:knob]", 180.0, 35.0, 320.0, 1.0)) : si.smooth(0.999);
    lpManual = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[04] Manual Brightness LP[unit:Hz][style:knob]", 8000.0, 2500.0, 14000.0, 10.0)) : si.smooth(0.999);
    interTrimDB = hgroup("Preamp Guitarix Configurable/[00] Knobs", hslider("[05] Manual Interstage Trim[unit:dB][style:knob]", 0.0, -18.0, 12.0, 0.1)) : si.smooth(0.999);

    // ---------- NEW: optional cathode sag ----------
    sagEnabled = hgroup("Preamp Guitarix Configurable/[15] Dynamics", checkbox("[40] Cathode Sag"));
    sagTau_knob = hgroup("Preamp Guitarix Configurable/[15] Dynamics", hslider("[41] Sag Time (ms)[style:knob]", 30.0, 5.0, 200.0, 1.0)) : si.smooth(0.999);
    sagAmt_knob = hgroup("Preamp Guitarix Configurable/[15] Dynamics", hslider("[42] Sag Amount[style:knob]", 0.15, 0.0, 0.5, 0.01)) : si.smooth(0.999);

    //                         Lab           Mesa  Fender JCM   Soldano Vox
    sagTau = choose5ap(sagTau_knob,          18.0, 50.0,  35.0, 25.0,   55.0) : si.smooth(0.999);
    sagAmt = choose5ap(sagAmt_knob,          0.22, 0.08,  0.15, 0.20,   0.18) : si.smooth(0.999);

    sag_alpha = exp(-1.0 / max(1.0, sagTau * 0.001 * ma.SR));
    sag_core(x) = x * max(0.1, 1.0 - sagAmt * (abs(x) : si.smooth(sag_alpha)));
    sag_block = _ <: _, sag_core : select2(sagEnabled);

    // ---------- NEW: optional bias offset ----------
    biasEnabled = hgroup("Preamp Guitarix Configurable/[15] Dynamics", checkbox("[43] Bias Offset (H2)"));
    biasOff_knob = hgroup("Preamp Guitarix Configurable/[15] Dynamics", hslider("[44] Bias Amount[style:knob]", -0.08, -0.3, 0.1, 0.001)) : si.smooth(0.999);

    //                          Lab            Mesa   Fender  JCM    Soldano Vox
    biasOff = choose5ap(biasOff_knob,         -0.12, -0.03,  -0.08, -0.10,  -0.05) : si.smooth(0.999);
    biasAdd = biasOff * float(biasEnabled);

    // ---------- independent topology ----------
    useStage2 = nbStages >= 2;
    useStage3 = nbStages >= 3;
    useStage4 = nbStages >= 4;
    useStage5 = nbStages >= 5;

    // ---------- defaults by musical profile ----------
    // In Lab mode (ampModel=0): use ToneProfile menus (choose6)
    // In preset mode (ampModel>0): override with amp-specific values

    // Gain ranges
    gainLo_lab = choose6(0.70, 1.00, 1.50, 2.50, 4.00, 5.00);
    gainHi_lab = choose6(2.00, 3.00, 4.50, 7.00, 12.00, 16.00);
    //                          Lab         Mesa   Fender JCM   Soldano Vox
    mainGain = rangePot(gainKnob,
        choose5ap(gainLo_lab,               5.00,  1.00,  2.00, 5.00,   1.50),
        choose5ap(gainHi_lab,               16.00, 4.00,  8.00, 14.00,  5.00));

    // Default gain placement per profile
    profileGP_lab = choose6(1, 1, 1, 2, 2, 2);
    profileGP = choose5ap(profileGP_lab, 2, 1, 1, 2, 1);
    gainPlacement = select2(gainPlacementMenu > 0, profileGP, gainPlacementMenu);

    // Default voicing per profile (Lab mode uses ToneProfile → Voicing mapping)
    profileVoicing_lab = choose6(1, 2, 3, 4, 6, 6);
    // In preset mode, we force a specific voicing index:
    // Mesa→6(Recto Modern), Fender→2(Balanced Clean), JCM→3(Vintage Crunch),
    // Soldano→5(Recto Vintage), Vox→2(Balanced Clean)
    voicing_forced = choose5ap(0, 6, 2, 3, 5, 2);
    voicing_lab = select2(voicingMenu > 0, profileVoicing_lab, voicingMenu);
    voicing = select2(ampModel > 0, voicing_lab, voicing_forced);

    // Voicing presets: HP, LP, attenuation per interstage
    // These use choose7v which maps the voicing index to values.
    // When ampModel>0, voicing is forced above, so choose7v returns the right values.
    // Additionally, we override the values for hi-gain correctness via choose5ap.
    hpDefault_v = choose7v(140.0, 65.0, 90.0, 130.0, 165.0, 190.0, 230.0);
    lpDefault_v = choose7v(8500.0, 12000.0, 11000.0, 9500.0, 8500.0, 7600.0, 7000.0);

    // CORRECTED: Mesa/Soldano HP was too high, LP too low in previous version.
    //                          Lab          Mesa     Fender   JCM      Soldano  Vox
    hpDefault = choose5ap(hpDefault_v,       120.0,   35.0,    80.0,    100.0,   90.0);
    lpDefault = choose5ap(lpDefault_v,       8500.0,  12000.0, 10000.0, 8500.0,  11000.0);

    // CORRECTED: Interstage attenuation. Previous values too aggressive for hi-gain.
    att12_v = choose7v(0.80, 0.90, 0.82, 0.70, 0.62, 0.65, 0.58);
    att23_v = choose7v(0.60, 0.85, 0.75, 0.62, 0.54, 0.60, 0.52);
    att34_v = choose7v(0.45, 0.80, 0.70, 0.55, 0.46, 0.48, 0.42);
    att45_v = choose7v(0.40, 0.75, 0.65, 0.50, 0.42, 0.44, 0.38);

    //                           Lab     Mesa  Fender JCM   Soldano Vox
    att12Default = choose5ap(att12_v,    0.85, 0.92,  0.82, 0.82,   0.88);
    att23Default = choose5ap(att23_v,    0.78, 0.88,  0.75, 0.75,   0.82);
    att34Default = choose5ap(att34_v,    0.70, 0.82,  0.68, 0.68,   0.82);
    att45Default = choose5ap(att45_v,    0.65, 0.75,  0.65, 0.62,   0.75);

    // With Use preset voicing ON, these come from voicing preset.
    // With it OFF, manual knobs override (only relevant in Lab mode).
    hp_auto = select2(usePresetVoicing, hpManual, hpDefault);
    lp_auto = select2(usePresetVoicing, lpManual, lpDefault);
    interTrim_auto = select2(usePresetVoicing, db2lin(interTrimDB), 1.0);
    // When ampModel > 0, always use the preset-driven values
    hp = select2(ampModel > 0, hp_auto, hpDefault);
    lp = select2(ampModel > 0, lp_auto, lpDefault);
    interTrim = select2(ampModel > 0, interTrim_auto, 1.0);

    // Input drive: boost for hi-gain profiles
    inputDrive_lab = db2lin(inputTrimDB) * choose6(1.00, 1.00, 1.10, 1.25, 1.55, 1.90);
    //                             Lab             Mesa Fender JCM  Soldano Vox
    inputDrive = choose5ap(inputDrive_lab,          1.80, 1.00, 1.20, 1.70, 1.10)
                 * db2lin(inputTrimDB);

    outputGain = rangePot(outputKnob, 0.05, 3.0);

    // ---------- gain and attenuation distribution ----------
    mildGain = sqrt(max(0.001, mainGain));

    g12Strategic = select2(gainPlacement == 1, att12Default, mainGain);
    g23Strategic = select2(gainPlacement == 2, att23Default, mainGain);
    g34Strategic = select2(gainPlacement == 3, att34Default, mainGain);

    g12 = select2(gainPlacement == 4, g12Strategic, att12Default * mildGain) * interTrim;
    g23 = select2(gainPlacement == 4, g23Strategic, att23Default * mildGain) * interTrim;
    g34 = select2(gainPlacement == 4, g34Strategic, att34Default * mildGain) * interTrim;
    g45 = select2(gainPlacement == 4, att45Default, att45Default * mildGain) * interTrim;

    // CORRECTED: Output compensation was too aggressive for hi-gain presets.
    stageComp_lab = ba.selectn(5, nbStages-1, (1.00, 0.86, 0.68, 0.52, 0.40));
    profileComp_lab = choose6(1.00, 0.92, 0.78, 0.60, 0.42, 0.32);
    //                              Lab                    Mesa Fender JCM  Soldano Vox
    stageComp = choose5ap(stageComp_lab,                   0.52, 0.90, 0.72, 0.50, 0.85);
    profileComp = choose5ap(profileComp_lab,               0.45, 0.92, 0.70, 0.42, 0.85);
    outComp = stageComp * profileComp;

    // ---------- interstage filters ----------
    filt12 = fi.highpass(1, max(35.0, hp*0.65)) : fi.lowpass(1, lp);
    filt23 = fi.highpass(1, hp) : fi.lowpass(1, lp);
    filt34 = fi.highpass(1, hp) : fi.lowpass(1, max(2500.0, lp*0.92));
    filt45 = fi.highpass(1, hp) : fi.lowpass(1, max(2500.0, lp*0.85));

    pre12 = *(g12) : filt12;
    pre23 = *(g23) : filt23;
    pre34 = *(g34) : filt34;
    pre45 = *(g45) : filt45;

    // ---------- tube selection ----------
    // Bias offset is added before the tube lookup when biasEnabled is ON
    tube_menu1 = hgroup("Preamp Guitarix Configurable/[20] Tube choices", nentry("[30] Stage 1 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu2 = hgroup("Preamp Guitarix Configurable/[20] Tube choices", nentry("[31] Stage 2 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu3 = hgroup("Preamp Guitarix Configurable/[20] Tube choices", nentry("[32] Stage 3 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu4 = hgroup("Preamp Guitarix Configurable/[20] Tube choices", nentry("[33] Stage 4 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu5 = hgroup("Preamp Guitarix Configurable/[20] Tube choices", nentry("[34] Stage 5 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));

    Stage1 = +(biasAdd) : (tube_menu1, (_ <: T1_12AX7, T1_12AT7, T1_12AU7, T1_6V6, T1_6DJ8, T1_6C16)) : ba.selectn(6);
    Stage2 = +(biasAdd) : (tube_menu2, (_ <: T2_12AX7, T2_12AT7, T2_12AU7, T2_6V6, T2_6DJ8, T2_6C16)) : ba.selectn(6);
    Stage3 = +(biasAdd) : (tube_menu3, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);
    Stage4 = +(biasAdd) : (tube_menu4, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);
    Stage5 = +(biasAdd) : (tube_menu5, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);

    // ---------- meters ----------
    pk_decay = exp(-1.0 / (0.5 * ma.SR));
    p_hold = max ~ (*(pk_decay));

    meter(x, m) = x : attach(_, abs(x) : p_hold : ba.linear2db : m);

    mtr_in = hbargraph("Preamp Guitarix Configurable/[90] Meters/[0] Input[unit:dB]", -60, 10);
    mtr_s1 = hbargraph("Preamp Guitarix Configurable/[90] Meters/[1] After Stage1[unit:dB]", -60, 10);
    mtr_12 = hbargraph("Preamp Guitarix Configurable/[90] Meters/[2] Inter 1-2[unit:dB]", -60, 10);
    mtr_23 = hbargraph("Preamp Guitarix Configurable/[90] Meters/[3] Inter 2-3[unit:dB]", -60, 10);
    mtr_34 = hbargraph("Preamp Guitarix Configurable/[90] Meters/[4] Inter 3-4[unit:dB]", -60, 10);
    mtr_45 = hbargraph("Preamp Guitarix Configurable/[90] Meters/[5] Inter 4-5[unit:dB]", -60, 10);
    mtr_out = hbargraph("Preamp Guitarix Configurable/[90] Meters/[6] Output[unit:dB]", -60, 10);

    m_in(x) = meter(x, mtr_in);
    m_s1(x) = meter(x, mtr_s1);
    m_12(x) = meter(x, mtr_12);
    m_23(x) = meter(x, mtr_23);
    m_34(x) = meter(x, mtr_34);
    m_45(x) = meter(x, mtr_45);
    m_out(x) = meter(x, mtr_out);
};

// Root UI group
preampMono = vgroup("Preamp Guitarix Configurable v5 Amp Presets", ba.bypass_fade(ma.SR/10, checkbox("[99] bypass"), preampMB));
preAmp = preampMono;
//process = preAmp;
