# Faust Amp Lab

## Overview
This project is a **100% Faust implementation of a greybox tube guitar amplifier simulation**. It is designed as an "Amp Lab," providing a highly customizable environment for experimenting with and shaping every aspect of the amplifier's signal chain. 

Unlike black-box models (e.g., pure machine learning profiles) that capture a single static snapshot of an amp, this greybox approach models the fundamental electrical behaviors of tube stages, tonestacks, and power amps. This allows for deep customization of individual parameters—such as tube bias, interstage gain, asymmetric clipping dynamics, and EQ topologies—giving you complete freedom to tweak classic amps or design your own signature tone from the ground up.

## Project Structure & Architecture

The amplifier simulation is highly modular, with the complete audio graph constructed from several dedicated DSP files located in the `dsp/` directory.

Each file is heavily documented with comments explaining the purpose of each component and parameter. 

### Main DSP Components

* **`IFCampSim.dsp`**
  The main entry point of the project. It chains together the various discrete modules (preamp, tonestack, power amp, cabinet, and reverb) into a cohesive amplifier simulation.

* **`IFCpreampGridCurrentCathodeBiasOptionnalDynStage.dsp`**
  A highly detailed, multi-stage tube preamp model. It features adjustable grid current emulation, cathode bias mechanics, and an optional dynamic asymmetric tube stage for highly responsive, touch-sensitive breakup. It includes pre-calibrated iconic "Amp Models" (Fender, Marshall, Vox, Boogie, etc.) as well as a fully unlocked "Custom" mode.

* **`IFCpreampGuitarix.dsp`**
  An alternative, pure Faust port of the popular Guitarix tube modeling algorithm, offering a different mathematical flavor of tube saturation.

* **`IFCtonestackMulti.dsp`**
  A collection of classic passive tonestack topologies (e.g., Bassman, JCM800, Twin Reverb). It includes algorithmic makeup gain to automatically compensate for the inherent signal attenuation of passive EQ circuits.

* **`IFCpowerAmp.dsp`**
  Simulates the push-pull dynamics, voltage sag, and speaker interaction resonance of a hardware tube power amplifier section.

* **`IFCcabSim.dsp`**
  Cabinet simulation utilizing Finite Impulse Response (FIR) filters based on real speaker measurements.

* **`IFCreverbMulti.dsp`**
  Built-in spatial effects to add room ambience or classic spring reverb textures to the final output.

### Scripts & Tooling

* **IR Conversion Python Script (`convert_ir.py`)**
  A utility script designed to take multiple `.wav` impulse responses (IRs) of guitar cabinets and automatically compile them into Faust FIR filter definitions. It intelligently wraps the filters in an optimized `ba.selectoutn` routing matrix, ensuring that the Faust compiler utilizes true "on-demand" execution. This ensures that only the actively selected speaker cabinet consumes CPU cycles, allowing you to load massive impulse banks without performance penalties.

## Ecosystem Integration
Because the core engine is written purely in Faust, it is incredibly portable. It can be compiled into:
- Standalone desktop applications
- Standard audio plugins (VST, AU, LV2)
- Web Audio Modules (WAM2) for browser-based usage (via `faust2wam` / `faustwasm`).

## Generating a Web Audio Module (WAM2) Version

There are two primary ways to compile this project into a browser-ready Web Audio Module:

### Method 1: Using the FAUST Online IDE (Manual Export)
1. Copy the DSP files into the [FAUST Web IDE](https://faustide.grame.fr/).
2. Open `IFCampSim.dsp` as the main compilation target. *(Note: You can choose which preamp model to use by commenting/uncommenting the relevant preamp `import("...")` line at the top of the file).*
3. Follow this online tutorial to export the code as a WAM: **[Faust to WAM2 Tutorial](https://docs.google.com/document/d/1HDEm4m_cD47YBuDilzGYiANYQDktj56Njyv0umGYO6o/edit?usp=sharing)**

### Method 2: Using the `faust2wam` CLI (Automated/Local)
This method is highly recommended if you are using an AI-powered source code editor or working locally from your terminal.

1. Clone or download the [`faust2wam` project repository](https://github.com/Fr0stbyteR/faust2wam).
2. Open your terminal, navigate into the `faust2wam` folder, and install the dependencies:
   ```bash
   npm install
   ```
3. Run the generator script pointing to the main DSP file and your desired output directory:
   ```bash
   node faust2wam.js ../AmpFaust_repoGithub/dsp/IFCampSim.dsp test/out
   ```
4. This will generate a runnable WAM inside the `test/out/` folder. Open `test/out/host/index.html` in your web browser to access the amp simulator's test host interface.
5. In the host page, you can try the amp with included dry guitar audio files or use your live instrument input. You can also select input/output devices and adjust I/O gains.

*Developer Note:* If you inspect the `index.html` and `index.js` files inside the generated `host/` folder, you can see exactly how the WAM is loaded, instantiated, and connected to the browser's Web Audio API graph. For more technical information regarding the architecture of Web Audio Modules, refer to this AES paper: **[Web Audio Modules 2.0: An Open Web Audio Plugin Standard](https://aes.org/publications/elibrary-page/?id=23110)**.

## Academic Publications
If you use this work in an academic context or wish to read more about the research behind these models, please reference the following publication regarding our Web Audio tube guitar amp simulations:

* **Michel Buffa, Jerome Lebrun. [Rocking the Web with Browser-based Simulations of Tube Guitar Amplifiers](https://hal.science/hal-04335875).** 
  *Journal of the Audio Engineering Society*, 2023, 71 (11), pp.753-768. 
  ⟨[10.17743/jaes.2022.0110](https://doi.org/10.17743/jaes.2022.0110)⟩. ⟨hal-04335875⟩
  *(PDF available at the HAL link above).*

  and the upcoming paper we will present at the **[International Faust Conference 2026](https://ifc26.i3s.univ-cotedazur.fr/)**
