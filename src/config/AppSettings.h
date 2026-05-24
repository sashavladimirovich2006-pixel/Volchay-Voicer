#pragma once

#include "dsp/EffectSettings.h"

#include <QVariantMap>

namespace volchay {

struct AppSettings {
    int inputDevice = -1;
    int outputDevice = -1;
    int sampleRate = 48000;
    int framesPerBuffer = 256;
    QVariantMap effects;

    QVariantMap toVariantMap() const;
    static AppSettings fromVariantMap(const QVariantMap& map);
    static AppSettings load();
    static bool save(const AppSettings& settings);
};

} // namespace volchay

