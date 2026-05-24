#include "audio/PortAudioRuntime.h"

#include <mutex>

#include <portaudio.h>
#include <spdlog/spdlog.h>

namespace volchay {
namespace {

std::mutex g_mutex;
int g_refCount = 0;
bool g_initialized = false;
QString g_error;

} // namespace

PortAudioRuntime::PortAudioRuntime()
{
    std::scoped_lock lock(g_mutex);
    if (!g_initialized) {
        const auto err = Pa_Initialize();
        if (err != paNoError) {
            g_error = QString::fromUtf8(Pa_GetErrorText(err));
            spdlog::error("PortAudio initialization failed: {}", g_error.toStdString());
        } else {
            g_initialized = true;
            g_error.clear();
            spdlog::info("PortAudio initialized");
        }
    }

    if (g_initialized) {
        ++g_refCount;
        m_ready = true;
    } else {
        m_error = g_error;
    }
}

PortAudioRuntime::~PortAudioRuntime()
{
    std::scoped_lock lock(g_mutex);
    if (!m_ready || !g_initialized) {
        return;
    }

    --g_refCount;
    if (g_refCount <= 0) {
        Pa_Terminate();
        g_initialized = false;
        g_refCount = 0;
        spdlog::info("PortAudio terminated");
    }
}

} // namespace volchay

