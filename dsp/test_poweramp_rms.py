import urllib.request
import os
import subprocess

dsp_code = """
import("stdfaust.lib");
import("IFCampSim.dsp");

// We want to measure the RMS of the signal when fully bypassed vs engaged
// We simply hook up a noise or sine wave or soundfile 
// and measure RMS.

process = powerAmp_stage;
"""

with open("test_poweramp_rms.dsp", "w") as f:
    f.write(dsp_code)
