import("stdfaust.lib");
import("IFCguitarixTubes.dsp");

preampMB = *(inputVolume)
    : chain
    : *(gain)
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

    // 3 chains de longueur croissante, sélectionnées par nstages
    // Note : le  réseau RC de couplage inter-étage 
    // est modélisé par le filtre fi.lowpass(1,6531.0)
    // Plaque étage N
    //  │
    // [Cc]  ← condensateur de couplage (typiquement 22 nF)
    //  │
    // [Rg]  ← résistance de grille du tube suivant (typiquement 1 MΩ)
    //  │
    // étage N+1 (Grille)
    // 
    // Ce réseau forme un filtre passe-bas dont la fréquence de coupure est :
    // fc = 1 / (2π × Cc × Rg)
    //    = 1 / (2π × 22nF × 1MΩ)
    //    = 7237 Hz
    // 
    // C'est cohérent avec le 6531 Hz utilisé dans le code. 
    //  - Adoucit le mordant et le "spit" des hautes fréquences générées par 
    //      la distorsion
    //  - écrase l'extrême aigu avant que le signal n'entre dans l'étage suivant.
    //  - Réduit l'aliasing potentiel transmis à l'étage suivant
    //  - Colore le son : chaque étage voit un signal déjà "arrondi" dans 
    //    l'aigu → la distorsion des étages 2 et 3 travaille sur un signal moins 
    //    riche en harmoniques → saturation plus progressive et "musicale"

    chain1 = Stage1;
    chain2 = Stage1 : *(preamp) : fi.lowpass(1,6531.0) : Stage2;
    chain3 = Stage1 : *(preamp) : fi.lowpass(1,6531.0) : Stage2 : *(preamp) : fi.lowpass(1,6531.0) : Stage3;

    chain = (nstages, (_ <: chain1, chain2, chain3)) : ba.selectn(3);
};

preampMono = hgroup("Preamp Guitarix", ba.bypass_fade(ma.SR/10, checkbox("bypass"), preampMB));

preAmp = preampMono;
//process = preAmp;

 