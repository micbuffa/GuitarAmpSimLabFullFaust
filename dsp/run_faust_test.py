import os
import subprocess

dsp_code = """
import("stdfaust.lib");
import("IFCpowerAmp.dsp");

// Generate a 0.05 RMS noise signal to simulate the tonestack output
test_signal = no.noise * 0.05;

// Measure RMS of input (Bypassed)
rms_in = test_signal : an.amp_follower_rms(0.5);

// Measure RMS of Output (Engaged)
rms_out = test_signal : powerAmp_stage : an.amp_follower_rms(0.5);

process = rms_in, rms_out;
"""

with open("measure_pa_rms.dsp", "w") as f:
    f.write(dsp_code)

# compile and run simply to get console output at t=1s
# faust2sndfile can output a wav file, or we can just use faust command line tools
with open("test_runner.cpp", "w") as f:
    f.write("""
// simple C++ wrapper to run Faust and print output
#include <iostream>
""")
