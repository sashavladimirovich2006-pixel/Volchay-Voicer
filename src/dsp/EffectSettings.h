#pragma once

#include <QVariantMap>

namespace volchay {

struct EffectSettings {
    bool bypass = false;
    double pitchSemitones = 0.0;
    double formantSemitones = 0.0;
    double robotMix = 0.0;
    double robotFrequencyHz = 45.0;
    double radioMix = 0.0;
    double echoMix = 0.0;
    double echoDelayMs = 120.0;
    double echoFeedback = 0.24;
    double noiseGateThresholdDb = -58.0;
    double compressorThresholdDb = -12.0;
    double compressorRatio = 3.0;
    double outputGainDb = 0.0;

    QVariantMap toVariantMap() const
    {
        return {
            {"bypass", bypass},
            {"pitchSemitones", pitchSemitones},
            {"formantSemitones", formantSemitones},
            {"robotMix", robotMix},
            {"robotFrequencyHz", robotFrequencyHz},
            {"radioMix", radioMix},
            {"echoMix", echoMix},
            {"echoDelayMs", echoDelayMs},
            {"echoFeedback", echoFeedback},
            {"noiseGateThresholdDb", noiseGateThresholdDb},
            {"compressorThresholdDb", compressorThresholdDb},
            {"compressorRatio", compressorRatio},
            {"outputGainDb", outputGainDb}
        };
    }

    static EffectSettings fromVariantMap(const QVariantMap& map)
    {
        EffectSettings settings;
        auto readBool = [&](const char* key, bool fallback) {
            return map.contains(key) ? map.value(key).toBool() : fallback;
        };
        auto readDouble = [&](const char* key, double fallback) {
            return map.contains(key) ? map.value(key).toDouble() : fallback;
        };

        settings.bypass = readBool("bypass", settings.bypass);
        settings.pitchSemitones = readDouble("pitchSemitones", settings.pitchSemitones);
        settings.formantSemitones = readDouble("formantSemitones", settings.formantSemitones);
        settings.robotMix = readDouble("robotMix", settings.robotMix);
        settings.robotFrequencyHz = readDouble("robotFrequencyHz", settings.robotFrequencyHz);
        settings.radioMix = readDouble("radioMix", settings.radioMix);
        settings.echoMix = readDouble("echoMix", settings.echoMix);
        settings.echoDelayMs = readDouble("echoDelayMs", settings.echoDelayMs);
        settings.echoFeedback = readDouble("echoFeedback", settings.echoFeedback);
        settings.noiseGateThresholdDb = readDouble("noiseGateThresholdDb", settings.noiseGateThresholdDb);
        settings.compressorThresholdDb = readDouble("compressorThresholdDb", settings.compressorThresholdDb);
        settings.compressorRatio = readDouble("compressorRatio", settings.compressorRatio);
        settings.outputGainDb = readDouble("outputGainDb", settings.outputGainDb);
        return settings;
    }
};

} // namespace volchay

