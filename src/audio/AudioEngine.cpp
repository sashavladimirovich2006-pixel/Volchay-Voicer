#include "audio/AudioEngine.h"

#include "audio/PortAudioRuntime.h"
#include "dsp/BasicEffects.h"
#include "logging/Logger.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <portaudio.h>
#include <spdlog/spdlog.h>

namespace volchay {
namespace {

int resolveDefaultInput()
{
    const auto dev = Pa_GetDefaultInputDevice();
    return dev == paNoDevice ? -1 : dev;
}

int resolveDefaultOutput()
{
    const auto dev = Pa_GetDefaultOutputDevice();
    return dev == paNoDevice ? -1 : dev;
}

} // namespace

AudioEngine::AudioEngine()
{
}

AudioEngine::~AudioEngine()
{
    stop();
}

AudioStartResult AudioEngine::start(const AudioStreamConfig& config, const EffectSettings& settings)
{
    AudioStartResult result;
    if (!m_runtime.isReady()) {
        result.error = m_runtime.error();
        return result;
    }

    stop();

    m_config = config;
    m_settings = settings;
    m_chain.setSettings(settings);
    m_chain.prepare({static_cast<double>(config.sampleRate), config.inputChannels, config.framesPerBuffer});

    const auto* inputInfo = Pa_GetDeviceInfo(config.inputDevice >= 0 ? config.inputDevice : resolveDefaultInput());
    const auto* outputInfo = Pa_GetDeviceInfo(config.outputDevice >= 0 ? config.outputDevice : resolveDefaultOutput());

    PaStreamParameters inputParams {};
    inputParams.device = config.inputDevice >= 0 ? config.inputDevice : resolveDefaultInput();
    inputParams.channelCount = config.inputChannels;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = inputInfo ? inputInfo->defaultLowInputLatency : 0.02;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters outputParams {};
    outputParams.device = config.outputDevice >= 0 ? config.outputDevice : resolveDefaultOutput();
    outputParams.channelCount = config.outputChannels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = outputInfo ? outputInfo->defaultLowOutputLatency : 0.02;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    if (inputParams.device < 0 || outputParams.device < 0) {
        result.error = QStringLiteral("Input or output device is unavailable.");
        m_error = result.error;
        return result;
    }

    const auto err = Pa_OpenStream(&m_stream,
        &inputParams,
        &outputParams,
        config.sampleRate,
        static_cast<unsigned long>(config.framesPerBuffer),
        paNoFlag,
        &AudioEngine::callback,
        this);
    if (err != paNoError) {
        result.error = QString::fromUtf8(Pa_GetErrorText(err));
        m_error = result.error;
        spdlog::error("PortAudio open stream failed: {}", result.error.toStdString());
        return result;
    }

    const auto startErr = Pa_StartStream(m_stream);
    if (startErr != paNoError) {
        result.error = QString::fromUtf8(Pa_GetErrorText(startErr));
        m_error = result.error;
        spdlog::error("PortAudio start stream failed: {}", result.error.toStdString());
        stop();
        return result;
    }

    m_running = true;
    m_error.clear();
    result.ok = true;

    inputInfo = Pa_GetDeviceInfo(inputParams.device);
    outputInfo = Pa_GetDeviceInfo(outputParams.device);
    if (inputInfo) {
        result.inputLatencyMs = inputInfo->defaultLowInputLatency * 1000.0;
    }
    if (outputInfo) {
        result.outputLatencyMs = outputInfo->defaultLowOutputLatency * 1000.0;
    }
    m_latencyMs = result.inputLatencyMs + result.outputLatencyMs;

    spdlog::info("Stream started: inputDevice={}, outputDevice={}, sampleRate={}, framesPerBuffer={}, inputChannels={}, outputChannels={}",
        inputParams.device,
        outputParams.device,
        config.sampleRate,
        config.framesPerBuffer,
        config.inputChannels,
        config.outputChannels);

    return result;
}

void AudioEngine::stop()
{
    if (m_stream) {
        Pa_StopStream(m_stream);
        Pa_CloseStream(m_stream);
        m_stream = nullptr;
        spdlog::info("Stream stopped");
    }
    m_running = false;
    m_latencyMs = 0.0;
}

bool AudioEngine::isRunning() const
{
    return m_running.load();
}

QString AudioEngine::error() const
{
    return m_error;
}

void AudioEngine::setSettings(const EffectSettings& settings)
{
    std::scoped_lock lock(m_mutex);
    m_settings = settings;
    m_chain.setSettings(settings);
}

EffectSettings AudioEngine::settings() const
{
    std::scoped_lock lock(m_mutex);
    return m_settings;
}

double AudioEngine::inputLevel() const
{
    return m_inputLevel.load();
}

double AudioEngine::outputLevel() const
{
    return m_outputLevel.load();
}

double AudioEngine::cpuLoad() const
{
    return m_cpuLoad.load();
}

double AudioEngine::latencyMs() const
{
    return m_latencyMs.load();
}

AudioStreamConfig AudioEngine::config() const
{
    return m_config;
}

int AudioEngine::callback(const void* input, void* output, unsigned long frameCount,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData)
{
    auto* engine = static_cast<AudioEngine*>(userData);
    if (!engine) {
        return paAbort;
    }

    const auto* in = static_cast<const float*>(input);
    auto* out = static_cast<float*>(output);
    engine->processBlock(in, out, static_cast<int>(frameCount), engine->m_config.inputChannels, engine->m_config.outputChannels);
    engine->m_cpuLoad.store(Pa_GetStreamCpuLoad(engine->m_stream));
    return paContinue;
}

void AudioEngine::zeroOutput(float* output, int frames, int channels)
{
    if (!output) {
        return;
    }
    std::fill(output, output + frames * channels, 0.0f);
}

void AudioEngine::processBlock(const float* input, float* output, int frames, int inputChannels, int outputChannels)
{
    if (!output) {
        return;
    }

    zeroOutput(output, frames, outputChannels);
    if (static_cast<int>(m_monoIn.size()) != frames) {
        m_monoIn.resize(frames);
        m_monoOut.resize(frames);
    }
    std::fill(m_monoIn.begin(), m_monoIn.end(), 0.0f);
    std::fill(m_monoOut.begin(), m_monoOut.end(), 0.0f);

    if (input) {
        for (int frame = 0; frame < frames; ++frame) {
            double sum = 0.0;
            for (int channel = 0; channel < inputChannels; ++channel) {
                sum += input[frame * inputChannels + channel];
            }
            m_monoIn[frame] = static_cast<float>(sum / std::max(1, inputChannels));
        }
        m_inputLevel.store(calculateRms(m_monoIn.data(), frames, 1));
        m_monoOut = m_monoIn;
    } else {
        m_inputLevel.store(0.0);
    }

    m_chain.process(m_monoOut.data(), frames, 1);
    m_outputLevel.store(calculateRms(m_monoOut.data(), frames, 1));

    for (int frame = 0; frame < frames; ++frame) {
        const auto sample = m_monoOut[frame];
        for (int channel = 0; channel < outputChannels; ++channel) {
            output[frame * outputChannels + channel] = sample;
        }
    }
}

} // namespace volchay
