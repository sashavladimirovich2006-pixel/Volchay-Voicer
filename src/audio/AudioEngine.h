#pragma once

#include "audio/AudioTypes.h"
#include "audio/PortAudioRuntime.h"
#include "dsp/EffectSettings.h"
#include "dsp/EffectsChain.h"

#include <QObject>
#include <QVariantMap>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include <portaudio.h>

namespace volchay {

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    AudioStartResult start(const AudioStreamConfig& config, const EffectSettings& settings);
    void stop();
    bool isRunning() const;
    QString error() const;

    void setSettings(const EffectSettings& settings);
    EffectSettings settings() const;

    double inputLevel() const;
    double outputLevel() const;
    double cpuLoad() const;
    double latencyMs() const;

    AudioStreamConfig config() const;

private:
    static int callback(const void* input, void* output, unsigned long frameCount,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

    void processBlock(const float* input, float* output, int frames, int inputChannels, int outputChannels);
    static void zeroOutput(float* output, int frames, int channels);

    AudioStreamConfig m_config;
    EffectSettings m_settings;
    EffectsChain m_chain;
    PaStream* m_stream = nullptr;
    mutable std::mutex m_mutex;
    std::atomic<double> m_inputLevel {0.0};
    std::atomic<double> m_outputLevel {0.0};
    std::atomic<double> m_cpuLoad {0.0};
    std::atomic<double> m_latencyMs {0.0};
    std::atomic<bool> m_running {false};
    QString m_error;
    PortAudioRuntime m_runtime;
    std::vector<float> m_monoIn;
    std::vector<float> m_monoOut;
    std::vector<float> m_outputScratch;
};

} // namespace volchay
