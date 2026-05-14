// ────────────────────────────────────────────────────────────
//  WAM Host – index.js
//  Audio source switching, device menus, gain + VU meters
// ────────────────────────────────────────────────────────────

/* ── DOM refs ───────────────────────────────────────────── */
const sourceSelect = document.getElementById('source-select');
const inputDeviceSelect = document.getElementById('input-device-select');
const outputDeviceSelect = document.getElementById('output-device-select');
const player = document.getElementById('player');
const mount = document.getElementById('mount');
const liveStatus = document.getElementById('live-status');
const statusText = document.getElementById('status-text');
const srDisplay = document.getElementById('sr-display');
const firefoxWarning =
    document.getElementById('firefox-warning');

// Gain controls
const inputGainPanel = document.getElementById('input-gain-panel');
const inputGainSlider = document.getElementById('input-gain');
const inputGainValue = document.getElementById('input-gain-value');
const outputGainSlider = document.getElementById('output-gain');
const outputGainValue = document.getElementById('output-gain-value');

// VU meter elements
const inputVuFill = document.getElementById('input-vu-fill');
const inputVuPeak = document.getElementById('input-vu-peak');
const outputVuFill = document.getElementById('output-vu-fill');
const outputVuPeak = document.getElementById('output-vu-peak');

/* ── State ──────────────────────────────────────────────── */
let audioContext = null;
let currentSource = null;
let liveStream = null;
let wamInstance = null;
let firstPluginInstance = null;
let mediaElSource = null;

// Audio graph nodes
let inputGainNode = null;
let outputGainNode = null;
let inputAnalyser = null;
let outputAnalyser = null;

// Firefox helpers
const isFirefox =
    navigator.userAgent.toLowerCase().includes('firefox');

if (isFirefox && firefoxWarning) {

    firefoxWarning.textContent =
        '(live output routing unsupported)';

    console.warn(
        '[Host] Firefox detected: AudioContext.setSinkId unsupported'
    );
}

let sourceSwitchToken = 0;

// VU meter state
let inputPeakLevel = 0;
let outputPeakLevel = 0;
let vuAnimFrame = null;

/* ── Helpers ────────────────────────────────────────────── */
function setStatus(msg) {
    statusText.textContent = msg;
}

function gainToDb(gain) {
    if (gain < 0.0001) return '-∞';
    return (20 * Math.log10(gain)).toFixed(1);
}

function resumeAudio() {
    if (audioContext) {
        console.log(
            `[Host] Resuming audio context (current state: ${audioContext.state})`
        );

        if (audioContext.state === 'suspended') {
            audioContext.resume().then(() =>
                console.log('[Host] Context resumed successfully')
            );
        }
    }
}

/* ── VU Meter engine ────────────────────────────────────── */
function getRmsLevel(analyser, dataArray) {
    analyser.getByteTimeDomainData(dataArray);

    let sum = 0;

    for (let i = 0; i < dataArray.length; i++) {
        const v = (dataArray[i] - 128) / 128;
        sum += v * v;
    }

    return Math.sqrt(sum / dataArray.length);
}

function updateVuMeter(fillEl, peakEl, rms, peakState) {

    const dbFS = rms > 0.0001
        ? 20 * Math.log10(rms)
        : -100;

    const pct = Math.max(
        0,
        Math.min(100, ((dbFS + 60) / 60) * 100)
    );

    fillEl.style.width = `${pct}%`;

    if (pct > peakState.value) {
        peakState.value = pct;
        peakState.holdFrames = 30;
        peakEl.style.opacity = '0.9';

    } else if (peakState.holdFrames > 0) {

        peakState.holdFrames--;

    } else {

        peakState.value = Math.max(
            0,
            peakState.value - 0.8
        );

        if (peakState.value < 1) {
            peakEl.style.opacity = '0';
        }
    }

    peakEl.style.left = `${peakState.value}%`;
}

const inputPeak = { value: 0, holdFrames: 0 };
const outputPeak = { value: 0, holdFrames: 0 };

let inputDataArray = null;
let outputDataArray = null;

function vuLoop() {

    if (
        inputAnalyser &&
        inputDataArray &&
        !inputGainPanel.classList.contains('hidden')
    ) {

        const rms = getRmsLevel(
            inputAnalyser,
            inputDataArray
        );

        updateVuMeter(
            inputVuFill,
            inputVuPeak,
            rms,
            inputPeak
        );
    }

    if (outputAnalyser && outputDataArray) {

        const rms = getRmsLevel(
            outputAnalyser,
            outputDataArray
        );

        updateVuMeter(
            outputVuFill,
            outputVuPeak,
            rms,
            outputPeak
        );

        if (
            !window._lastVuLog ||
            Date.now() - window._lastVuLog > 3000
        ) {
            window._lastVuLog = Date.now();
            console.log(`[Host] VU Meter Sync: Input RMS = ${inputVuFill.style.width}, Output RMS = ${outputVuFill.style.width}`);
        }
    }

    vuAnimFrame = requestAnimationFrame(vuLoop);
}

/* ── 1. Populate audio file list from manifest ──────────── */
async function loadAudioFileList() {

    try {

        const resp = await fetch(
            '../assets/audio/audioFiles.json'
        );

        if (!resp.ok) {
            throw new Error(`HTTP ${resp.status}`);
        }

        const files = await resp.json();

        files.forEach(name => {

            const opt = document.createElement('option');

            opt.value = `../assets/audio/${name}`;
            opt.textContent = `🎵 ${name}`;

            sourceSelect.appendChild(opt);
        });

        if (files.length) {
            setStatus(`${files.length} audio file(s) available`);
        }

    } catch (err) {

        console.warn(
            'Could not load audioFiles.json',
            err
        );

        setStatus('No audio files found');
    }
}

/* ── 2. Device enumeration ─────────────────────────────── */

/**
 * Sur Firefox, enumerateDevices() ne retourne les labels que si la
 * permission microphone a déjà été accordée. On fait un getUserMedia
 * minimal, on libère le stream, puis on énumère.
 */
async function requestMicPermissionThenEnumerate() {
    try {
        const stream = await navigator.mediaDevices.getUserMedia({ audio: true, video: false });
        // Libérer immédiatement — on voulait juste débloquer les labels
        stream.getTracks().forEach(t => t.stop());
    } catch (err) {
        // L'utilisateur a refusé ou pas de micro : on énumère quand même
        console.warn('[Host] Permission mic refusée ou indisponible:', err.message);
    }
    await enumerateDevices();
}

async function enumerateDevices() {
    try {
        const devices = await navigator.mediaDevices.enumerateDevices();
        const savedInput = inputDeviceSelect.value;

        inputDeviceSelect.innerHTML = '';

        const inputs = devices.filter(
            d => d.kind === 'audioinput'
        );

        if (!inputs.length) {
            inputDeviceSelect.add(
                new Option('— no input devices —', '')
            );
        }

        inputs.forEach((d, i) => {

            inputDeviceSelect.add(
                new Option(
                    d.label || `Input ${i + 1}`,
                    d.deviceId
                )
            );
        });

        if (
            [...inputDeviceSelect.options]
                .some(o => o.value === savedInput)
        ) {
            inputDeviceSelect.value = savedInput;
        }

        const savedOutput = outputDeviceSelect.value;

        outputDeviceSelect.innerHTML = '';

        const outputs = devices.filter(
            d => d.kind === 'audiooutput'
        );

        if (!outputs.length) {
            outputDeviceSelect.add(
                new Option('— no output devices —', '')
            );
        }

        outputs.forEach((d, i) => {

            outputDeviceSelect.add(
                new Option(
                    d.label || `Output ${i + 1}`,
                    d.deviceId
                )
            );
        });

        if (
            [...outputDeviceSelect.options]
                .some(o => o.value === savedOutput)
        ) {
            outputDeviceSelect.value = savedOutput;
        }

    } catch (err) {

        console.error(
            'Device enumeration error:',
            err
        );
    }
}

navigator.mediaDevices.addEventListener(
    'devicechange',
    () => {

        enumerateDevices();

        setStatus(
            'Audio devices changed — lists refreshed'
        );
    }
);

/* ── 3. Source switching ───────────────────────────────── */
// Firefox workaround: hidden muted audio element to keep stream active
const dummyAudio = document.createElement('audio');
dummyAudio.muted = true;
dummyAudio.style.display = 'none';
document.body.appendChild(dummyAudio);

// Firefox workaround: ScriptProcessor to force real-time pulling
let keepAliveNode = null;

function startKeepAlive() {
    if (keepAliveNode) return;
    try {
        // ScriptProcessor is deprecated but very effective at forcing Firefox to keep the clock running
        keepAliveNode = audioContext.createScriptProcessor(1024, 1, 1);
        keepAliveNode.onaudioprocess = () => {
            // Aggressive resume if context is suspended
            if (audioContext && audioContext.state === 'suspended') {
                audioContext.resume();
            }
        };
        keepAliveNode.connect(audioContext.destination);
        console.log('[Host] Keep-alive ScriptProcessor started');
    } catch (e) {
        console.warn('[Host] Could not start ScriptProcessor keep-alive', e);
    }
}

async function disconnectCurrentSource() {
    if (currentSource) {
        try {
            currentSource.disconnect();
        } catch (_) { }

        currentSource = null;
    }

    if (liveStream) {
        liveStream.getTracks().forEach(t => t.stop());
        liveStream = null;

        // Clear dummy audio
        dummyAudio.srcObject = null;

        // Firefox/macOS CoreAudio workaround
        if (isFirefox) {
            await new Promise(r => setTimeout(r, 250));
        }
    }

    player.pause();
    player.style.display = '';
    liveStatus.style.display = 'none';
}

async function switchSource() {

    if (!audioContext || !firstPluginInstance) {
        return;
    }

    const token = ++sourceSwitchToken;

    await disconnectCurrentSource();

    if (token !== sourceSwitchToken) {
        return;
    }

    const val = sourceSelect.value;
    const isLive = val === '__live__';

    inputGainPanel.classList.toggle(
        'hidden',
        !isLive
    );

    if (isLive) {
        const deviceId = inputDeviceSelect.value;

        const constraints = {
            audio: {
                deviceId: deviceId ? deviceId : undefined,
                echoCancellation: false,
                noiseSuppression: false,
                autoGainControl: false
            }
        };

        try {
            console.log('[Host] getUserMedia constraints:', constraints);

            liveStream = await navigator.mediaDevices.getUserMedia(constraints);

            if (token !== sourceSwitchToken) {
                liveStream.getTracks().forEach(t => t.stop());
                return;
            }

            // Firefox workaround: route to audio element (muted)
            dummyAudio.srcObject = liveStream;
            dummyAudio.muted = true;
            dummyAudio.play().catch(() => { });

            currentSource = audioContext.createMediaStreamSource(liveStream);

            // Fix for Firefox: keep context clock alive
            startKeepAlive();

            if (isFirefox) {
                await audioContext.resume();
            }

            // Route through full chain (connected at init)
            currentSource.connect(inputAnalyser);
            // And Amp -> Output Gain (already done in init)

            player.style.display = 'none';
            liveStatus.style.display = 'flex';
            setStatus('🎤 Live input active');
            console.log('[Host] Live input connected');

        } catch (err) {
            console.error('getUserMedia error:', err);
            setStatus(`Mic error: ${err.message}`);
        }

    } else {

        player.src = val;

        if (!mediaElSource) {
            mediaElSource =
                audioContext
                    .createMediaElementSource(player);
        }

        currentSource = mediaElSource;

        // Unify routing: MP3 also goes through inputGainNode and inputAnalyser
        currentSource.connect(inputGainNode);

        player.style.display = '';
        liveStatus.style.display = 'none';

        setStatus(`Playing: ${val.split('/').pop()}`);

        if (audioContext.state === 'suspended') {
            await audioContext.resume();
        }
    }
}

inputDeviceSelect.addEventListener(
    'change',
    () => {

        resumeAudio();

        if (sourceSelect.value === '__live__') {
            switchSource();
        }
    }
);

sourceSelect.addEventListener(
    'change',
    () => {

        resumeAudio();
        switchSource();
    }
);

/* ── 4. Gain slider handlers ───────────────────────────── */
inputGainSlider.addEventListener(
    'input',
    () => {

        resumeAudio();

        const g =
            parseFloat(inputGainSlider.value);

        if (inputGainNode) {
            inputGainNode.gain.value = g;
        }

        inputGainValue.textContent =
            `${gainToDb(g)} dB`;
    }
);

outputGainSlider.addEventListener(
    'input',
    () => {

        resumeAudio();

        const g =
            parseFloat(outputGainSlider.value);

        if (outputGainNode) {

            outputGainNode.gain.value = g;

            console.log(
                `[Host] Output gain changed to: ${g}`
            );
        }

        outputGainValue.textContent =
            `${gainToDb(g)} dB`;
    }
);

/* ── 5. Output device switching ────────────────────────── */
outputDeviceSelect.addEventListener(
    'change',
    async () => {

        const deviceId =
            outputDeviceSelect.value;

        console.log(
            `[Host] Output switch request: ${deviceId}`
        );

        try {

            // Chrome / Edge
            if (
                audioContext &&
                typeof audioContext.setSinkId === 'function'
            ) {

                await audioContext.setSinkId(
                    deviceId || ""
                );

                setStatus(
                    `Output → ${outputDeviceSelect
                        .selectedOptions[0]?.text
                    || 'Default'
                    }`
                );

                return;
            }

            // Firefox fallback
            if (
                player &&
                typeof player.setSinkId === 'function'
            ) {

                await player.setSinkId(
                    deviceId || ""
                );

                setStatus(
                    'Firefox: output changed for file playback only'
                );

                return;
            }

            setStatus(
                'Firefox does not support WebAudio output switching'
            );

        } catch (err) {

            console.error(
                '[Host] Output switch error:',
                err
            );

            setStatus(
                `Output error: ${err.message}`
            );
        }
    }
);

/* ── 6. Main init ──────────────────────────────────────── */
(async () => {

    setStatus('Loading audio file list…');

    await loadAudioFileList();

    setStatus('Listing devices…');
    await requestMicPermissionThenEnumerate(); // Permission mic d'abord (Firefox)

    setStatus('Initialising WAM host…');

    audioContext = new AudioContext();

    srDisplay.textContent =
        `${audioContext.sampleRate} Hz`;

    // Input chain
    inputGainNode =
        audioContext.createGain();

    inputAnalyser =
        audioContext.createAnalyser();

    inputAnalyser.fftSize = 2048;

    inputDataArray =
        new Uint8Array(inputAnalyser.fftSize);

    inputGainNode.connect(inputAnalyser);

    // Output chain
    outputGainNode =
        audioContext.createGain();

    outputAnalyser =
        audioContext.createAnalyser();

    outputAnalyser.fftSize = 2048;

    outputDataArray =
        new Uint8Array(outputAnalyser.fftSize);

    outputGainNode.connect(outputAnalyser);
    outputAnalyser.connect(audioContext.destination);

    // Init WamEnv
    const { initializeWamHost } =
        await import('../sdk/index.js');

    const [hostGroupId] =
        await initializeWamHost(audioContext);

    // plugins...
    // Load all plugins dynamically
    const [
        { default: TunerMachine },
        { default: Deathgate },
        { default: AutoWah },
        { default: TS9Overdrive },
        { default: StonePhaserStereo },
        { default: WAMChorus },
        { default: WAM },
        { default: PingPongDelay },
        { default: GreyHole }
    ] = await Promise.all([
        import('./plugins/tuner_machine/src/index.js'),
        import('./plugins/deathgate/index.js'),
        import('./plugins/AutoWahMB/index.js'),
        import('./plugins/TS9_OverdriveFaustGenerated/index.js'),
        import('./plugins/StonePhaserStereo/index.js'),
        import('./plugins/WAMChorusMB/index.js'),
        import('../index.js'),
        import('https://mainline.i3s.unice.fr/wam2/packages/faustPingPongDelay/plugin/index.js'),
        import('./plugins/greyhole/index.js')
    ]);

    // Instantiate all plugins
    const [
        tunerInst,
        deathgateInst,
        autoWahInst,
        ts9Inst,
        stonePhaserStereoInst,
        chorusInst,
        ampInst,
        pingpongInst,
        greyholeInst,
    ] = await Promise.all([
        TunerMachine.createInstance(hostGroupId, audioContext),
        Deathgate.createInstance(hostGroupId, audioContext),
        AutoWah.createInstance(hostGroupId, audioContext),
        TS9Overdrive.createInstance(hostGroupId, audioContext),
        StonePhaserStereo.createInstance(hostGroupId, audioContext),
        WAMChorus.createInstance(hostGroupId, audioContext),
        WAM.createInstance(hostGroupId, audioContext),
        PingPongDelay.createInstance(hostGroupId, audioContext),
        GreyHole.createInstance(hostGroupId, audioContext)
    ]);

    // Connect audio graph - FULL CHAIN
    inputAnalyser.connect(tunerInst.audioNode);
    tunerInst.audioNode.connect(deathgateInst.audioNode);
    deathgateInst.audioNode.connect(autoWahInst.audioNode);
    autoWahInst.audioNode.connect(ts9Inst.audioNode);
    ts9Inst.audioNode.connect(stonePhaserStereoInst.audioNode);
    stonePhaserStereoInst.audioNode.connect(chorusInst.audioNode);
    chorusInst.audioNode.connect(ampInst.audioNode);
    ampInst.audioNode.connect(pingpongInst.audioNode);
    pingpongInst.audioNode.connect(greyholeInst.audioNode);
    
    // Final output to destination and VU meters
    greyholeInst.audioNode.connect(audioContext.destination);
    greyholeInst.audioNode.connect(outputGainNode);

    firstPluginInstance = tunerInst;
    wamInstance = ampInst;

    // Create and mount GUIs
    const guis = await Promise.all([
        tunerInst.createGui(),
        deathgateInst.createGui(),
        autoWahInst.createGui(),
        ts9Inst.createGui(),
        stonePhaserStereoInst.createGui(),
        chorusInst.createGui(),
        ampInst.createGui(),
        pingpongInst.createGui(),
        greyholeInst.createGui()
    ]);

    mount.innerHTML = '';

    // ── Structure de présentation (identique à index2.html) ──────────────
    const mainCol = document.createElement('div');
    mainCol.className = 'main-column';

    // Ligne du haut : Tuner (index 0) + Ampli (index 6) côte à côte
    const ampRow = document.createElement('div');
    ampRow.className = 'amp-row';

    const tunerWrap = document.createElement('div');
    tunerWrap.className = 'pedal-wrapper';
    tunerWrap.appendChild(guis[0]);   // tuner
    ampRow.appendChild(tunerWrap);

    ampRow.appendChild(guis[6]);      // ampli (pas de pedal-wrapper pour qu'il garde sa taille naturelle)
    mainCol.appendChild(ampRow);

    // Ligne du bas : toutes les pédales (sauf tuner idx=0 et ampli idx=6)
    const effectsRow = document.createElement('div');
    effectsRow.className = 'effects-row';

    const pedalIndices = [1, 2, 3, 4, 5, 7, 8]; // gate, wah, ts9, phaser, chorus, pingpong, greyhole
    pedalIndices.forEach(i => {
        const wrap = document.createElement('div');
        wrap.className = 'pedal-wrapper';
        wrap.appendChild(guis[i]);
        effectsRow.appendChild(wrap);
    });

    mainCol.appendChild(effectsRow);
    mount.appendChild(mainCol);

    // Harmonisation des hauteurs
    function scalePlugin(gui, targetHeight) {
        setTimeout(() => {
            const h = gui.getBoundingClientRect().height || gui.offsetHeight || 300;
            if (h > 0) {
                gui.style.zoom = targetHeight / h;
                const parent = gui.parentElement;
                if (parent && parent.classList.contains('pedal-wrapper')) {
                    parent.style.flex = '0 0 auto';
                    parent.style.width = 'auto';
                    parent.style.height = 'auto';
                }
            }
        }, 100);
    }

    requestAnimationFrame(() => {
        const ampHeight = guis[6].getBoundingClientRect().height || 450;
        scalePlugin(guis[0], ampHeight);                    // tuner à la hauteur de l'ampli
        pedalIndices.forEach(i => scalePlugin(guis[i], 180)); // pédales à 180 px
    });

    // Robust bypass helper using WAM-standard methods with retry logic
    const setPluginBypass = async (inst, bypassed) => {
        let info = null;
        for (let i = 0; i < 5; i++) {
            try {
                info = await inst.audioNode.getParameterInfo();
                if (info && Object.keys(info).length > 0) break;
            } catch (e) { }
            await new Promise(r => setTimeout(r, 200));
        }

        if (!info) {
            console.warn(`[Host] Could not get parameters for ${inst.descriptor?.name}. Skipping bypass.`);
            return;
        }

        const pIds = Object.keys(info);
        const bypassParam = pIds.find(p =>
            p.toLowerCase().endsWith('/bypass') ||
            p.toLowerCase() === 'enabled' ||
            p.toLowerCase() === 'bypass'
        );

        if (bypassParam) {
            const isEnabledStyle = bypassParam.toLowerCase().includes('enabled');
            const val = isEnabledStyle ? (bypassed ? 0 : 1) : (bypassed ? 1 : 0);

            try {
                await inst.audioNode.setParameterValues({
                    [bypassParam]: { id: bypassParam, value: val, normalized: false }
                });
                console.log(`[Host] ${inst.descriptor.name}: set ${bypassParam} to ${val} (${bypassed ? 'Bypassed' : 'Active'})`);
            } catch (e) {
                console.error(`[Host] Error setting ${bypassParam} for ${inst.descriptor.name}`, e);
            }
        } else {
            console.log(`[Host] No bypass param found for ${inst.descriptor.name}. Params:`, pIds.slice(0, 5));
        }
    };

    // Turn off effects (bypass) using standard WAM API AFTER GUIs are created
    setTimeout(async () => {
        console.log('[Host] Initializing plugin states (Bypassing by default)...');

        await setPluginBypass(tunerInst, true);
        await setPluginBypass(deathgateInst, false); // Ensure gate is active but threshold low

        try {
            await deathgateInst.audioNode.setParameterValues({
                "/deathgate/NoiseGate": { id: "/deathgate/NoiseGate", value: -120, normalized: false }
            });
        } catch (e) { }

        await setPluginBypass(autoWahInst, true);
        await setPluginBypass(ts9Inst, true);
        await setPluginBypass(stonePhaserStereoInst, true);
        await setPluginBypass(chorusInst, true);
        await setPluginBypass(pingpongInst, true);
        await setPluginBypass(greyholeInst, true);

        console.log('[Host] Initial plugin states set.');
    }, 1000);

    // Start VU meter animation
    vuLoop();

    setStatus('Ready — choose an audio source');
    // Resume AudioContext on any click anywhere on the page
    document.addEventListener('click', () => { resumeAudio(); }, { once: false });
    document.addEventListener('keydown', () => { resumeAudio(); }, { once: false });
    player.onplay = () => { audioContext.resume(); };

    await enumerateDevices();

    // Default to first audio file if available
    if (sourceSelect.options.length > 1) {
        sourceSelect.selectedIndex = 1;
        await switchSource();
    }
})();
