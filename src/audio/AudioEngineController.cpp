#include "audio/AudioEngineController.h"

#include "logging/Logger.h"
#include "presets/PresetModel.h"

#include <QVariant>
#include <spdlog/spdlog.h>

namespace volchay {

AudioEngineController::AudioEngineController(QObject* parent)
    : QObject(parent)
{
    m_metricTimer.setInterval(33);
    connect(&m_metricTimer, &QTimer::timeout, this, [this] { emitMetrics(); });
    m_metricTimer.start();
}

bool AudioEngineController::running() const
{
    return m_running;
}

QString AudioEngineController::errorMessage() const
{
    return m_errorMessage;
}

double AudioEngineController::inputLevel() const
{
    return m_engine.inputLevel();
}

double AudioEngineController::outputLevel() const
{
    return m_engine.outputLevel();
}

double AudioEngineController::cpuLoad() const
{
    return m_engine.cpuLoad();
}

double AudioEngineController::latencyMs() const
{
    return m_engine.latencyMs();
}

int AudioEngineController::selectedInputDevice() const
{
    return m_settings.inputDevice;
}

int AudioEngineController::selectedOutputDevice() const
{
    return m_settings.outputDevice;
}

int AudioEngineController::sampleRate() const
{
    return m_settings.sampleRate;
}

int AudioEngineController::framesPerBuffer() const
{
    return m_settings.framesPerBuffer;
}

QVariantMap AudioEngineController::effectSettings() const
{
    return m_settings.effects;
}

void AudioEngineController::refreshDevices()
{
    emit settingsChanged();
}

bool AudioEngineController::start()
{
    const auto settings = EffectSettings::fromVariantMap(m_settings.effects);
    AudioStreamConfig config;
    config.inputDevice = m_settings.inputDevice;
    config.outputDevice = m_settings.outputDevice;
    config.sampleRate = m_settings.sampleRate;
    config.framesPerBuffer = m_settings.framesPerBuffer;
    config.inputChannels = 1;
    config.outputChannels = 2;

    const auto result = m_engine.start(config, settings);
    if (!result.ok) {
        m_errorMessage = result.error;
        emit errorMessageChanged();
        spdlog::error("Unable to start engine: {}", result.error.toStdString());
        return false;
    }

    m_errorMessage.clear();
    m_running = true;
    emit runningChanged();
    emit errorMessageChanged();
    emitMetrics();
    return true;
}

void AudioEngineController::stop()
{
    m_engine.stop();
    if (m_running) {
        m_running = false;
        emit runningChanged();
    }
    emitMetrics();
}

void AudioEngineController::toggle()
{
    if (m_running) {
        stop();
    } else {
        start();
    }
}

void AudioEngineController::applyPreset(const QVariantMap& preset)
{
    setEffectSettings(preset);
}

void AudioEngineController::setEffectParameter(const QString& key, const QVariant& value)
{
    auto effects = m_settings.effects;
    effects.insert(key, value);
    setEffectSettings(effects);
}

QVariantMap AudioEngineController::snapshot() const
{
    return m_settings.toVariantMap();
}

void AudioEngineController::restore(const QVariantMap& snapshot)
{
    m_settings = AppSettings::fromVariantMap(snapshot);
    syncEngineSettings();
    emit settingsChanged();
}

void AudioEngineController::setSelectedInputDevice(int deviceIndex)
{
    if (m_settings.inputDevice == deviceIndex) {
        return;
    }
    m_settings.inputDevice = deviceIndex;
    emit settingsChanged();
}

void AudioEngineController::setSelectedOutputDevice(int deviceIndex)
{
    if (m_settings.outputDevice == deviceIndex) {
        return;
    }
    m_settings.outputDevice = deviceIndex;
    emit settingsChanged();
}

void AudioEngineController::setSampleRate(int sampleRate)
{
    if (m_settings.sampleRate == sampleRate) {
        return;
    }
    m_settings.sampleRate = sampleRate;
    emit settingsChanged();
}

void AudioEngineController::setFramesPerBuffer(int framesPerBuffer)
{
    if (m_settings.framesPerBuffer == framesPerBuffer) {
        return;
    }
    m_settings.framesPerBuffer = framesPerBuffer;
    emit settingsChanged();
}

void AudioEngineController::setEffectSettings(const QVariantMap& settings)
{
    m_settings.effects = settings;
    syncEngineSettings();
    emit settingsChanged();
}

void AudioEngineController::emitMetrics()
{
    emit metricsChanged();
}

void AudioEngineController::syncEngineSettings()
{
    m_engine.setSettings(EffectSettings::fromVariantMap(m_settings.effects));
}

} // namespace volchay
