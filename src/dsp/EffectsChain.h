#pragma once

#include "dsp/AudioEffect.h"

#include <memory>
#include <mutex>
#include <vector>

namespace volchay {

class EffectsChain {
public:
    EffectsChain();

    void prepare(const AudioProcessContext& context);
    void setSettings(const EffectSettings& settings);
    EffectSettings settings() const;
    void reset();
    void process(float* interleaved, int frames, int channels);

private:
    mutable std::mutex m_mutex;
    AudioProcessContext m_context;
    EffectSettings m_settings;
    std::vector<std::unique_ptr<AudioEffect>> m_effects;
};

} // namespace volchay

