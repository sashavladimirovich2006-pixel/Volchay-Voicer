#include "dsp/EffectsChain.h"

#include "dsp/BasicEffects.h"

namespace volchay {

EffectsChain::EffectsChain()
{
    m_effects.push_back(std::make_unique<PitchShiftEffect>());
    m_effects.push_back(std::make_unique<RobotEffect>());
    m_effects.push_back(std::make_unique<RadioEffect>());
    m_effects.push_back(std::make_unique<EchoEffect>());
    m_effects.push_back(std::make_unique<DynamicsEffect>());
}

void EffectsChain::prepare(const AudioProcessContext& context)
{
    std::scoped_lock lock(m_mutex);
    m_context = context;
    for (auto& effect : m_effects) {
        effect->prepare(context);
        effect->setSettings(m_settings);
    }
}

void EffectsChain::setSettings(const EffectSettings& settings)
{
    std::scoped_lock lock(m_mutex);
    m_settings = settings;
    for (auto& effect : m_effects) {
        effect->setSettings(m_settings);
    }
}

EffectSettings EffectsChain::settings() const
{
    std::scoped_lock lock(m_mutex);
    return m_settings;
}

void EffectsChain::reset()
{
    std::scoped_lock lock(m_mutex);
    for (auto& effect : m_effects) {
        effect->reset();
    }
}

void EffectsChain::process(float* interleaved, int frames, int channels)
{
    std::scoped_lock lock(m_mutex);
    if (m_settings.bypass) {
        return;
    }
    for (auto& effect : m_effects) {
        effect->process(interleaved, frames, channels);
    }
}

} // namespace volchay
