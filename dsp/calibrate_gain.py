import os
import subprocess
import re

signal_path = "/Users/michelbuffa/Documents/Recherche/WAMs/NAM_NEURAL_AMP_MODELER/AMP_FAUST/SwankyAmpFaust/SwankyAmpFaust/data/signal.wav"
base_dir = "/Users/michelbuffa/Documents/Recherche/WAMs/NAM_NEURAL_AMP_MODELER/AMP_FAUST/AmpFullFaust/AmpFaust_repoGithub/dsp"

def get_rms(wav_file):
    res = subprocess.run(["ffmpeg", "-i", wav_file, "-filter:a", "volumedetect", "-f", "null", "/dev/null"], capture_output=True, text=True)
    match = re.search(r"mean_volume:\s+([-0-9.]+)\s+dB", res.stderr)
    if match:
        return float(match.group(1))
    return None

with open(os.path.join(base_dir, "IFCpreampGuitarix.dsp"), "r") as f:
    orig_code = f.read()

rms_values = []
for i in range(5):
    modified_code = orig_code.replace(
        "nstages = hgroup(\"Preamp Guitarix\", nentry(\"Nb Stages[style:menu{'1 stage':0;'2 stages':1;'3 stages':2;'4 stages':3;'5 stages':4}]\", 2, 0, 4, 1));",
        f"nstages = {i};"
    )
    # make sure process is uncommented
    modified_code = modified_code.replace('//process = preAmp;', 'process = preAmp;')
    
    tmp_dsp = os.path.join(base_dir, f"tmp_{i}.dsp")
    with open(tmp_dsp, "w") as f:
        f.write(modified_code)
    
    out_wav = os.path.join(base_dir, f"out_{i}.wav")
    print(f"Compiling for {i+1} stages...")
    subprocess.run(["faust2sndfile", tmp_dsp], check=True)
    
    exe_path = os.path.join(base_dir, f"tmp_{i}")
    print(f"Running for {i+1} stages...")
    subprocess.run([exe_path, signal_path, out_wav], check=True)
    
    rms = get_rms(out_wav)
    print(f"Stages {i+1} RMS: {rms} dB")
    rms_values.append(rms)

if None not in rms_values:
    target_rms = rms_values[0]
    gains_linear = []
    for rms in rms_values:
        diff_db = target_rms - rms
        gain_lin = 10 ** (diff_db / 20.0)
        gains_linear.append(gain_lin)

    print("stage_comp_gain values:", gains_linear)
else:
    print("Error getting RMS")
