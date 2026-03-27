import numpy as np
from scipy.io import wavfile
import sys
import os

def convert():
    if len(sys.argv) < 2:
        print("Usage: python convert_multi_ir.py <file1.wav> <file2.wav> ... [taps]")
        return

    # Gestion des arguments : fichiers vs nombre de taps
    try:
        taps = int(sys.argv[-1])
        files = sys.argv[1:-1]
    except ValueError:
        taps = 640
        files = sys.argv[1:]

    if not files:
        print("❌ Error : No WAV file provided.")
        return

    fir_definitions = []
    fir_names = []
    fir_gains = []
    cab_names = []

    for idx, wav_path in enumerate(files):
        if not os.path.exists(wav_path):
            print(f"⚠️ Ignoré (absent) : {wav_path}")
            continue

        try:
            sample_rate, data = wavfile.read(wav_path)
            if len(data.shape) > 1: data = np.mean(data, axis=1)
            data = data.astype(np.float32)
            if np.max(np.abs(data)) > 0: data = data / np.max(np.abs(data))
            
            coeffs = data[:taps]
            if len(coeffs) < taps:
                coeffs = np.pad(coeffs, (0, taps - len(coeffs)))

            # Calculate RMS power for volume matching
            # Amplitude gain is approx sqrt(sum(coeffs^2))
            gain_factor = np.sqrt(np.sum(coeffs**2))
            fir_gains.append(f"{gain_factor:.8f}")

            coeffs_str = ", ".join([f"{v:.8f}" for v in coeffs])
            base_name = os.path.splitext(os.path.basename(wav_path))[0]
            
            # Storage of the definitions
            fir_definitions.append(f"fir_{idx} = fi.fir(({coeffs_str})); // {base_name}")
            fir_names.append(f"fir_{idx}")
            cab_names.append(base_name)
            print(f"✅ IR processed : {base_name}")

        except Exception as e:
            print(f"❌ Error on {wav_path} : {e}")

    # Preparation of the strings for the FAUST template
    all_firs = "\n".join(fir_definitions)
    selector_list = ", ".join(fir_names)
    fir_gains_str = ", ".join(fir_gains)
    num_cabs = len(fir_names)

    # Drop down menu
    safe_names = [name.replace("'", "") for name in cab_names]
    menu_items = ";".join([f"'{name}':{i}" for i, name in enumerate(safe_names)])
    menu_style = f"[style:menu{{{menu_items}}}]"

    faust_template = f"""import("stdfaust.lib");

// --- IMPULSE BANK ---
{all_firs}

// --- INTERFACE ---
bypass = hgroup("Cabinet Simulator", checkbox("Bypass[stratus:0]"));
selected_cab = hgroup("Cabinet Simulator", nentry("Cabinet Select[stratus:1]{menu_style}", 0, 0, {num_cabs - 1}, 1));
mix = hgroup("Cabinet Simulator", hslider("Mix (Dry/Wet)[stratus:2]", 1, 0, 1, 0.01)) : si.smoo;
gain = hgroup("Cabinet Simulator", hslider("Output (dB)[stratus:3]", 0, -20, 10, 0.1)) : ba.db2linear : si.smoo;

// --- ROUTING ---
// Send the input to all FIR, and select the output signal
cab_gain = {fir_gains_str} : ba.selectn({num_cabs}, selected_cab);

cab_process = _ <: (dry, wet) : + : *(gain)
with {{
    dry = *(1 - mix);
    wet = _ <: ({selector_list}) : ba.selectn({num_cabs}, selected_cab) : *(mix);
}};

bypassed_process = _ * cab_gain;

process = _ <: select2(bypass, cab_process, bypassed_process);
"""

    with open("MultiCab_Sim.dsp", "w") as f:
        f.write(faust_template)
    
    print(f"\n🚀 Finished ! 'MultiCab_Sim.dsp' generated with {num_cabs} cabs.")

if __name__ == "__main__":
    convert()