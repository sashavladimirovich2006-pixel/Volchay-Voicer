#pragma once

#include <QString>

namespace volchay {

class PortAudioRuntime {
public:
    PortAudioRuntime();
    ~PortAudioRuntime();

    bool isReady() const { return m_ready; }
    QString error() const { return m_error; }

private:
    bool m_ready = false;
    QString m_error;
};

} // namespace volchay

