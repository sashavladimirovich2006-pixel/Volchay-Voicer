#include "audio/AudioDeviceModel.h"

#include <portaudio.h>
#include <spdlog/spdlog.h>

namespace volchay {

AudioDeviceModel::AudioDeviceModel(Direction direction, QObject* parent)
    : QAbstractListModel(parent)
    , m_direction(direction)
{
    refresh();
}

int AudioDeviceModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_devices.size();
}

QVariant AudioDeviceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_devices.size()) {
        return {};
    }

    const auto& device = m_devices[index.row()];
    switch (role) {
    case DeviceIndexRole:
        return device.deviceIndex;
    case DisplayNameRole:
        return QString("%1 - %2").arg(device.hostApi, device.name);
    case HostApiRole:
        return device.hostApi;
    case ChannelsRole:
        return m_direction == Direction::Input ? device.maxInputChannels : device.maxOutputChannels;
    case DefaultRole:
        return m_direction == Direction::Input ? device.isDefaultInput : device.isDefaultOutput;
    default:
        return {};
    }
}

QHash<int, QByteArray> AudioDeviceModel::roleNames() const
{
    return {
        {DeviceIndexRole, "deviceIndex"},
        {DisplayNameRole, "displayName"},
        {HostApiRole, "hostApi"},
        {ChannelsRole, "channels"},
        {DefaultRole, "isDefault"}
    };
}

int AudioDeviceModel::count() const
{
    return m_devices.size();
}

int AudioDeviceModel::defaultModelIndex() const
{
    for (int i = 0; i < m_devices.size(); ++i) {
        const auto& device = m_devices[i];
        if ((m_direction == Direction::Input && device.isDefaultInput)
            || (m_direction == Direction::Output && device.isDefaultOutput)) {
            return i;
        }
    }
    return m_devices.isEmpty() ? -1 : 0;
}

void AudioDeviceModel::refresh()
{
    beginResetModel();
    m_devices.clear();

    if (!m_runtime.isReady()) {
        spdlog::error("Cannot refresh audio devices: {}", m_runtime.error().toStdString());
        endResetModel();
        emit devicesChanged();
        return;
    }

    const auto count = Pa_GetDeviceCount();
    const auto defaultInput = Pa_GetDefaultInputDevice();
    const auto defaultOutput = Pa_GetDefaultOutputDevice();

    for (PaDeviceIndex i = 0; i < count; ++i) {
        const auto* info = Pa_GetDeviceInfo(i);
        if (!info) {
            continue;
        }

        const bool acceptsDirection = m_direction == Direction::Input
            ? info->maxInputChannels > 0
            : info->maxOutputChannels > 0;
        if (!acceptsDirection) {
            continue;
        }

        const auto* host = Pa_GetHostApiInfo(info->hostApi);
        AudioDeviceInfo device;
        device.deviceIndex = i;
        device.name = QString::fromUtf8(info->name);
        device.hostApi = host ? QString::fromUtf8(host->name) : QStringLiteral("Unknown API");
        device.maxInputChannels = info->maxInputChannels;
        device.maxOutputChannels = info->maxOutputChannels;
        device.defaultSampleRate = info->defaultSampleRate;
        device.isDefaultInput = i == defaultInput;
        device.isDefaultOutput = i == defaultOutput;
        m_devices.push_back(device);
    }

    spdlog::info("Refreshed {} {} audio devices",
        m_devices.size(),
        m_direction == Direction::Input ? "input" : "output");
    endResetModel();
    emit devicesChanged();
}

int AudioDeviceModel::deviceIndexAt(int modelIndex) const
{
    if (modelIndex < 0 || modelIndex >= m_devices.size()) {
        return -1;
    }
    return m_devices[modelIndex].deviceIndex;
}

} // namespace volchay

