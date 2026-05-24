#pragma once

#include "audio/AudioTypes.h"
#include "audio/PortAudioRuntime.h"

#include <QAbstractListModel>
#include <QVector>

namespace volchay {

class AudioDeviceModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY devicesChanged)
    Q_PROPERTY(int defaultModelIndex READ defaultModelIndex NOTIFY devicesChanged)

public:
    enum class Direction {
        Input,
        Output
    };

    enum Roles {
        DeviceIndexRole = Qt::UserRole + 1,
        DisplayNameRole,
        HostApiRole,
        ChannelsRole,
        DefaultRole
    };

    explicit AudioDeviceModel(Direction direction, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    int defaultModelIndex() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE int deviceIndexAt(int modelIndex) const;

signals:
    void devicesChanged();

private:
    Direction m_direction;
    PortAudioRuntime m_runtime;
    QVector<AudioDeviceInfo> m_devices;
};

} // namespace volchay

