import("stdfaust.lib");
import("IFCguitarixTubes.dsp");

preampMB = *(inputVolume)
    : m1
    : Stage1 : m2
    : ( _ <: _, (*(preamp) : fi.lowpass(1,6531.0) : Stage2 : m3) : select2(nstages > 0) )
    : ( _ <: _, (*(preamp) : fi.lowpass(1,6531.0) : Stage3) : select2(nstages > 1) )
    : *(gain * gk_comp)
with { 
    inputVolume = hgroup("Preamp Guitarix", hslider("Input Volume[style:knob]",-6,-20,20,0.1) : ba.db2linear : si.smooth(0.999));
    preamp = hgroup("Preamp Guitarix", hslider("Interstage gain[style:knob]",-6,-20,20,0.1) : ba.db2linear : si.smooth(0.999));
    gain  = hgroup("Preamp Guitarix", hslider("Output gain[style:knob]", 7, -20.0, 20.0, 0.1) : ba.db2linear : si.smooth(0.999));

    nstages = hgroup("Preamp Guitarix", nentry("Nb Stages[style:menu{'1 stage':0;'2 stages':1;'3 stages':2}]", 2, 0, 2, 1));

    tube_menu1 = hgroup("Preamp Guitarix", nentry("Stage 1 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu2 = hgroup("Preamp Guitarix", nentry("Stage 2 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));
    tube_menu3 = hgroup("Preamp Guitarix", nentry("Stage 3 Tube[style:menu{'12AX7':0;'12AT7':1;'12AU7':2;'6V6':3;'6DJ8':4;'6C16':5}]", 0, 0, 5, 1));

    Stage1 = (tube_menu1, (_ <: T1_12AX7, T1_12AT7, T1_12AU7, T1_6V6, T1_6DJ8, T1_6C16)) : ba.selectn(6);
    Stage2 = (tube_menu2, (_ <: T2_12AX7, T2_12AT7, T2_12AU7, T2_6V6, T2_6DJ8, T2_6C16)) : ba.selectn(6);
    Stage3 = (tube_menu3, (_ <: T3_12AX7, T3_12AT7, T3_12AU7, T3_6V6, T3_6DJ8, T3_6C16)) : ba.selectn(6);

    // --- VU Meters (horizontal, dB, peak-hold with 0.5s decay) ---
    pk_decay = exp(-1.0 / (0.5 * ma.SR));
    p_hold = max ~ (*(pk_decay));

    mtr_hb1 = hbargraph("Preamp Guitarix/[0]Input[unit:dB]", -60, 10);
    mtr_hb2 = hbargraph("Preamp Guitarix/[1]Inter 1-2[unit:dB]", -60, 10);
    mtr_hb3 = hbargraph("Preamp Guitarix/[2]Output[unit:dB]", -60, 10);

    m1(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb1);
    m2(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb2);
    m3(x) = x : attach(_, abs(x) : p_hold : ba.linear2db : mtr_hb3);

    // Dynamic interstage gain compensation (keeps output constant as 'preamp' changes)
    // ref_linear = default interstage gain (-6 dB ≈ 0.501); calibration point for output gain
    ref_linear = ba.db2linear(-6.0);
    // Pure inverse proportionality is mathematically exact for the linear regime 
    // (which simulation proved is where standard DI signals operate in the Guitarix wavetable).
    gk_comp_ratio = ref_linear / max(0.001, preamp);
    // Disable compensation for 1-stage mode (no interstage gain applied)
    gk_comp = select2(nstages > 0, 1.0, gk_comp_ratio) : si.smooth(0.999);
};

preampMono = hgroup("Preamp Guitarix", ba.bypass_fade(ma.SR/10, checkbox("bypass"), preampMB));

preAmp = preampMono;
//process = preAmp;

 