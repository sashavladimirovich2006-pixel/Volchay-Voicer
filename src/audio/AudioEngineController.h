#pragma once

#include "audio/AudioEngine.h"
#include "audio/AudioDeviceModel.h"
#include "config/AppSettings.h"

#include <QTimer>

namespace volchay {

class AudioEngineController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(double inputLevel READ inputLevel NOTIFY metricsChanged)
    Q_PROPERTY(double outputLevel READ outputLevel NOTIFY metricsChanged)
    Q_PROPERTY(double cpuLoad READ cpuLoad NOTIFY metricsChanged)
    Q_PROPERTY(double latencyMs READ latencyMs NOTIFY metricsChanged)
    Q_PROPERTY(int selectedInputDevice READ selectedInputDevice WRITE setSelectedInputDevice NOTIFY settingsChanged)
    Q_PROPERTY(int selectedOutputDevice READ selectedOutputDevice WRITE setSelectedOutputDevice NOTIFY settingsChanged)
    Q_PROPERTY(int sampleRate READ sampleRate WRITE setSampleRate NOTIFY settingsChanged)
    Q_PROPERTY(int framesPerBuffer READ framesPerBuffer WRITE setFramesPerBuffer NOTIFY settingsChanged)
    Q_PROPERTY(QVariantMap effectSettings READ effectSettings WRITE setEffectSettings NOTIFY settingsChanged)

public:
    explicit AudioEngineController(QObject* parent = nullptr);

    bool running() const;
    QString errorMessage() const;
    double inputLevel() const;
    double outputLevel() const;
    double cpuLoad() const;
    double latencyMs() const;
    int selectedInputDevice() const;
    int selectedOutputDevice() const;
    int sampleRate() const;
    int framesPerBuffer() const;
    QVariantMap effectSettings() const;

    Q_INVOKABLE void refreshDevices();
    Q_INVOKABLE bool start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void applyPreset(const QVariantMap& preset);
    Q_INVOKABLE void setEffectParameter(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariantMap snapshot() const;
    Q_INVOKABLE void restore(const QVariantMap& snapshot);

    void setSelectedInputDevice(int deviceIndex);
    void setSelectedOutputDevice(int deviceIndex);
    void setSampleRate(int sampleRate);
    void setFramesPerBuffer(int framesPerBuffer);
    void setEffectSettings(const QVariantMap& settings);

signals:
    void runningChanged();
    void errorMessageChanged();
    void metricsChanged();
    void settingsChanged();

private:
    void emitMetrics();
    void syncEngineSettings();

    AudioEngine m_engine;
    AppSettings m_settings;
    QTimer m_metricTimer;
    QString m_errorMessage;
    bool m_running = false;
};

} // namespace volchay

