#pragma once

#include <QString>

namespace volchay {

struct AudioDeviceInfo {
    int deviceIndex = -1;
    QString name;
    QString hostApi;
    int maxInputChannels = 0;
    int maxOutputChannels = 0;
    double defaultSampleRate = 48000.0;
    bool isDefaultInput = false;
    bool isDefaultOutput = false;
};

struct AudioStreamConfig {
    int inputDevice = -1;
    int outputDevice = -1;
    int sampleRate = 48000;
    int framesPerBuffer = 256;
    int inputChannels = 1;
    int outputChannels = 2;
};

struct AudioStartResult {
    bool ok = false;
    QString error;
    double inputLatencyMs = 0.0;
    double outputLatencyMs = 0.0;
};

} // namespace volchay

