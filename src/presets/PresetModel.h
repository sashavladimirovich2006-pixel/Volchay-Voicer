#pragma once

#include "dsp/EffectSettings.h"

#include <QAbstractListModel>
#include <QVector>

namespace volchay {

class PresetModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DescriptionRole,
        SettingsRole
    };

    explicit PresetModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap preset(int index) const;
    Q_INVOKABLE QString presetName(int index) const;
    Q_INVOKABLE QString presetDescription(int index) const;

private:
    struct PresetEntry {
        QString name;
        QString description;
        EffectSettings settings;
    };

    QVector<PresetEntry> m_presets;
};

} // namespace volchay
