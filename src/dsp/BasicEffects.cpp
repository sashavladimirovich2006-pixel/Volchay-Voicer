#include "dsp/BasicEffects.h"

#include <algorithm>
#include <cmath>

#include <rubberband/RubberBandLiveShifter.h>
#include <spdlog/spdlog.h>

namespace volchay {
namespace {

constexpr double TwoPi = 6.28318530717958647692;

double semitonesToScale(double semitones)
{
    return std::pow(2.0, semitones / 12.0);
}

double mixClamped(double value)
{
    return std::clamp(value, 0.0, 1.0);
}

} // namespace

double dbToLinear(double db)
{
    return std::pow(10.0, db / 20.0);
}

float clampSample(float value)
{
    return std::clamp(value, -1.0f, 1.0f);
}

double calculateRms(const float* interleaved, int frames, int channels)
{
    if (!interleaved || frames <= 0 || channels <= 0) {
        return 0.0;
    }

    double sum = 0.0;
    const int total = frames * channels;
    for (int i = 0; i < total; ++i) {
        sum += static_cast<double>(interleaved[i]) * interleaved[i];
    }
    return std::sqrt(sum / static_cast<double>(total));
}

void PitchShiftEffect::prepare(const AudioProcessContext& context)
{
    m_context = context;
    rebuildShifter();
}

void PitchShiftEffect::setSettings(const EffectSettings& settings)
{
    m_settings = settings;
    updateRatios();
}

void PitchShiftEffect::reset()
{
    for (auto& channel : m_pendingInput) {
        channel.clear();
    }
    for (auto& channel : m_pendingOutput) {
        channel.clear();
    }
    if (m_shifter) {
        m_shifter->reset();
    }
}

void PitchShiftEffect::rebuildShifter()
{
    const auto channels = std::max(1, m_context.channels);
    const auto options = RubberBand::RubberBandLiveShifter::OptionWindowShort
        | RubberBand::RubberBandLiveShifter::OptionFormantPreserved
        | RubberBand::RubberBandLiveShifter::OptionChannelsTogether;

    m_shifter = std::make_unique<RubberBand::RubberBandLiveShifter>(
        static_cast<size_t>(m_context.sampleRate),
        static_cast<size_t>(channels),
        options);
    m_shifter->setDebugLevel(0);
    m_blockSize = static_cast<int>(m_shifter->getBlockSize());

    m_pendingInput.assign(channels, {});
    m_pendingOutput.assign(channels, {});
    m_inputBlock.assign(channels, std::vector<float>(m_blockSize, 0.0f));
    m_outputBlock.assign(channels, std::vector<float>(m_blockSize, 0.0f));
    m_inputPtrs.resize(channels);
    m_outputPtrs.resize(channels);
    for (int channel = 0; channel < channels; ++channel) {
        m_inputPtrs[channel] = m_inputBlock[channel].data();
        m_outputPtrs[channel] = m_outputBlock[channel].data();
    }
    updateRatios();
    spdlog::info("Rubber Band LiveShifter prepared: sampleRate={}, channels={}, blockSize={}",
        m_context.sampleRate,
        channels,
        m_blockSize);
}

void PitchShiftEffect::updateRatios()
{
    if (!m_shifter) {
        return;
    }

    const auto pitchScale = semitonesToScale(m_settings.pitchSemitones);
    const auto formantScale = std::abs(m_settings.formantSemitones) < 0.001
        ? 0.0
        : semitonesToScale(m_settings.formantSemitones);
    if (std::abs(pitchScale - m_lastPitchScale) > 0.0001) {
        m_shifter->setPitchScale(pitchScale);
        m_lastPitchScale = pitchScale;
    }
    if (std::abs(formantScale - m_lastFormantScale) > 0.0001) {
        m_shifter->setFormantScale(formantScale);
        m_lastFormantScale = formantScale;
    }
}

void PitchShiftEffect::process(float* interleaved, int frames, int channels)
{
    if (!interleaved || !m_shifter || frames <= 0 || channels <= 0 || m_blockSize <= 0) {
        return;
    }
    if (std::abs(m_settings.pitchSemitones) < 0.001 && std::abs(m_settings.formantSemitones) < 0.001) {
        return;
    }

    const int activeChannels = std::min(channels, static_cast<int>(m_pendingInput.size()));
    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < activeChannels; ++channel) {
            m_pendingInput[channel].push_back(interleaved[frame * channels + channel]);
        }
    }

    while (!m_pendingInput.empty() && static_cast<int>(m_pendingInput[0].size()) >= m_blockSize) {
        for (int channel = 0; channel < activeChannels; ++channel) {
            for (int i = 0; i < m_blockSize; ++i) {
                m_inputBlock[channel][i] = m_pendingInput[channel].front();
                m_pendingInput[channel].pop_front();
            }
            std::fill(m_outputBlock[channel].begin(), m_outputBlock[channel].end(), 0.0f);
        }

        updateRatios();
        m_shifter->shift(m_inputPtrs.data(), m_outputPtrs.data());

        for (int channel = 0; channel < activeChannels; ++channel) {
            m_pendingOutput[channel].insert(
                m_pendingOutput[channel].end(),
                m_outputBlock[channel].begin(),
                m_outputBlock[channel].end());
        }
    }

    int framesWritten = 0;
    while (framesWritten < frames
        && !m_pendingOutput.empty()
        && !m_pendingOutput[0].empty()) {
        for (int channel = 0; channel < activeChannels; ++channel) {
            interleaved[framesWritten * channels + channel] = m_pendingOutput[channel].front();
            m_pendingOutput[channel].pop_front();
        }
        ++framesWritten;
    }
}

void RobotEffect::prepare(const AudioProcessContext& context)
{
    m_sampleRate = context.sampleRate;
    reset();
}

void RobotEffect::setSettings(const EffectSettings& settings)
{
    m_settings = settings;
}

void RobotEffect::reset()
{
    m_phase = 0.0;
}

void RobotEffect::process(float* interleaved, int frames, int channels)
{
    const auto mix = mixClamped(m_settings.robotMix);
    if (!interleaved || mix <= 0.001) {
        return;
    }

    const auto frequency = std::clamp(m_settings.robotFrequencyHz, 5.0, 220.0);
    const auto phaseStep = TwoPi * frequency / m_sampleRate;
    for (int frame = 0; frame < frames; ++frame) {
        const auto carrier = static_cast<float>(std::sin(m_phase));
        for (int channel = 0; channel < channels; ++channel) {
            auto& sample = interleaved[frame * channels + channel];
            sample = static_cast<float>((1.0 - mix) * sample + mix * sample * carrier);
        }
        m_phase += phaseStep;
        if (m_phase > TwoPi) {
            m_phase -= TwoPi;
        }
    }
}

void RadioEffect::prepare(const AudioProcessContext& context)
{
    m_sampleRate = context.sampleRate;
    m_lowState.assign(context.channels, 0.0f);
    m_highState.assign(context.channels, 0.0f);
    m_prevInput.assign(context.channels, 0.0f);
}

void RadioEffect::setSettings(const EffectSettings& settings)
{
    m_settings = settings;
}

void RadioEffect::reset()
{
    std::fill(m_lowState.begin(), m_lowState.end(), 0.0f);
    std::fill(m_highState.begin(), m_highState.end(), 0.0f);
    std::fill(m_prevInput.begin(), m_prevInput.end(), 0.0f);
}

void RadioEffect::process(float* interleaved, int frames, int channels)
{
    const auto mix = mixClamped(m_settings.radioMix);
    if (!interleaved || mix <= 0.001) {
        return;
    }

    const auto lowpassHz = 3200.0;
    const auto highpassHz = 280.0;
    const auto lpAlpha = std::clamp((TwoPi * lowpassHz) / (TwoPi * lowpassHz + m_sampleRate), 0.0, 1.0);
    const auto hpAlpha = std::clamp(m_sampleRate / (TwoPi * highpassHz + m_sampleRate), 0.0, 1.0);

    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            const auto index = frame * channels + channel;
            const auto dry = interleaved[index];
            m_lowState[channel] += static_cast<float>(lpAlpha * (dry - m_lowState[channel]));
            m_highState[channel] = static_cast<float>(hpAlpha * (m_highState[channel] + m_lowState[channel] - m_prevInput[channel]));
            m_prevInput[channel] = m_lowState[channel];
            const auto bandLimited = std::tanh(m_highState[channel] * 2.2f);
            interleaved[index] = static_cast<float>((1.0 - mix) * dry + mix * bandLimited);
        }
    }
}

void EchoEffect::prepare(const AudioProcessContext& context)
{
    m_sampleRate = context.sampleRate;
    m_channels = std::max(1, context.channels);
    const auto maxDelayFrames = static_cast<int>(m_sampleRate * 1.25);
    m_delay.assign(maxDelayFrames * m_channels, 0.0f);
    m_writeIndex = 0;
}

void EchoEffect::setSettings(const EffectSettings& settings)
{
    m_settings = settings;
}

void EchoEffect::reset()
{
    std::fill(m_delay.begin(), m_delay.end(), 0.0f);
    m_writeIndex = 0;
}

void EchoEffect::process(float* interleaved, int frames, int channels)
{
    const auto mix = mixClamped(m_settings.echoMix);
    if (!interleaved || mix <= 0.001 || m_delay.empty()) {
        return;
    }

    const int delayFrames = std::clamp(
        static_cast<int>(m_sampleRate * m_settings.echoDelayMs / 1000.0),
        1,
        static_cast<int>(m_delay.size() / m_channels) - 1);
    const auto feedback = std::clamp(m_settings.echoFeedback, 0.0, 0.85);
    const int ringFrames = static_cast<int>(m_delay.size() / m_channels);

    for (int frame = 0; frame < frames; ++frame) {
        const int readFrame = (m_writeIndex - delayFrames + ringFrames) % ringFrames;
        for (int channel = 0; channel < channels; ++channel) {
            const auto index = frame * channels + channel;
            const auto ringWrite = m_writeIndex * m_channels + std::min(channel, m_channels - 1);
            const auto ringRead = readFrame * m_channels + std::min(channel, m_channels - 1);
            const auto dry = interleaved[index];
            const auto delayed = m_delay[ringRead];
            m_delay[ringWrite] = clampSample(static_cast<float>(dry + delayed * feedback));
            interleaved[index] = clampSample(static_cast<float>((1.0 - mix) * dry + mix * delayed));
        }
        m_writeIndex = (m_writeIndex + 1) % ringFrames;
    }
}

void DynamicsEffect::prepare(const AudioProcessContext&)
{
}

void DynamicsEffect::setSettings(const EffectSettings& settings)
{
    m_settings = settings;
}

void DynamicsEffect::reset()
{
}

void DynamicsEffect::process(float* interleaved, int frames, int channels)
{
    if (!interleaved) {
        return;
    }

    const auto gateThreshold = dbToLinear(m_settings.noiseGateThresholdDb);
    const auto compressorThreshold = dbToLinear(m_settings.compressorThresholdDb);
    const auto ratio = std::clamp(m_settings.compressorRatio, 1.0, 20.0);
    const auto gain = dbToLinear(m_settings.outputGainDb);
    const int total = frames * channels;

    for (int i = 0; i < total; ++i) {
        auto sample = static_cast<double>(interleaved[i]);
        const auto absSample = std::abs(sample);
        if (absSample < gateThreshold) {
            sample *= 0.08;
        }

        const auto level = std::abs(sample);
        if (level > compressorThreshold && level > 0.0) {
            const auto exceeded = level - compressorThreshold;
            const auto compressed = compressorThreshold + exceeded / ratio;
            sample = std::copysign(compressed, sample);
        }

        interleaved[i] = clampSample(static_cast<float>(sample * gain));
    }
}

} // namespace volchay
