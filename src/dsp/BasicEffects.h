#pragma once

#include "dsp/AudioEffect.h"

#include <rubberband/RubberBandLiveShifter.h>

#include <memory>
#include <deque>
#include <vector>

namespace volchay {

class PitchShiftEffect final : public AudioEffect {
public:
    void prepare(const AudioProcessContext& context) override;
    void setSettings(const EffectSettings& settings) override;
    void reset() override;
    void process(float* interleaved, int frames, int channels) override;

private:
    void rebuildShifter();
    void updateRatios();

    EffectSettings m_settings;
    AudioProcessContext m_context;
    std::unique_ptr<RubberBand::RubberBandLiveShifter> m_shifter;
    int m_blockSize = 0;
    double m_lastPitchScale = 1.0;
    double m_lastFormantScale = 0.0;
    std::vector<std::deque<float>> m_pendingInput;
    std::vector<std::deque<float>> m_pendingOutput;
    std::vector<std::vector<float>> m_inputBlock;
    std::vector<std::vector<float>> m_outputBlock;
    std::vector<const float*> m_inputPtrs;
    std::vector<float*> m_outputPtrs;
};

class RobotEffect final : public AudioEffect {
public:
    void prepare(const AudioProcessContext& context) override;
    void setSettings(const EffectSettings& settings) override;
    void reset() override;
    void process(float* interleaved, int frames, int channels) override;

private:
    EffectSettings m_settings;
    double m_sampleRate = 48000.0;
    double m_phase = 0.0;
};

class RadioEffect final : public AudioEffect {
public:
    void prepare(const AudioProcessContext& context) override;
    void setSettings(const EffectSettings& settings) override;
    void reset() override;
    void process(float* interleaved, int frames, int channels) override;

private:
    EffectSettings m_settings;
    double m_sampleRate = 48000.0;
    std::vector<float> m_lowState;
    std::vector<float> m_highState;
    std::vector<float> m_prevInput;
};

class EchoEffect final : public AudioEffect {
public:
    void prepare(const AudioProcessContext& context) override;
    void setSettings(const EffectSettings& settings) override;
    void reset() override;
    void process(float* interleaved, int frames, int channels) override;

private:
    EffectSettings m_settings;
    double m_sampleRate = 48000.0;
    int m_channels = 1;
    std::vector<float> m_delay;
    int m_writeIndex = 0;
};

class DynamicsEffect final : public AudioEffect {
public:
    void prepare(const AudioProcessContext& context) override;
    void setSettings(const EffectSettings& settings) override;
    void reset() override;
    void process(float* interleaved, int frames, int channels) override;

private:
    EffectSettings m_settings;
};

double dbToLinear(double db);
float clampSample(float value);
double calculateRms(const float* interleaved, int frames, int channels);

} // namespace volchay
