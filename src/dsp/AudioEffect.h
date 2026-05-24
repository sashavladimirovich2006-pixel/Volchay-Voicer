#pragma once

#include "dsp/EffectSettings.h"

namespace volchay {

struct AudioProcessContext {
    double sampleRate = 48000.0;
    int channels = 1;
    int maxBlockFrames = 256;
};

class AudioEffect {
public:
    virtual ~AudioEffect() = default;
    virtual void prepare(const AudioProcessContext& context) = 0;
    virtual void setSettings(const EffectSettings& settings) = 0;
    virtual void reset() = 0;
    virtual void process(float* interleaved, int frames, int channels) = 0;
};

} // namespace volchay

