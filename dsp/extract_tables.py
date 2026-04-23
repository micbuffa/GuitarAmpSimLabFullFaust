import re
import numpy as np

with open('IFCguitarixTubes.dsp', 'r') as f:
    content = f.read()

tables = {}
matches = re.finditer(r'wf_([a-zA-Z0-9_]+)\s*=\s*waveform{([^}]+)};', content)
for m in matches:
    name = m.group(1)
    vals = [float(x) for x in m.group(2).split(',')]
    tables[name] = vals

print("Found tables:", list(tables.keys()))
