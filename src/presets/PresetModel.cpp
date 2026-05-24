#include "presets/PresetModel.h"

namespace volchay {

PresetModel::PresetModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_presets = {
        {
            QStringLiteral("Clean Voice"),
            QStringLiteral("Natural speech with light dynamics control."),
            EffectSettings{}
        },
        {
            QStringLiteral("Женский голос"),
            QStringLiteral("Повышенный pitch и formant для более светлого и естественного тембра."),
            [] {
                EffectSettings settings;
                settings.pitchSemitones = 5.0;
                settings.formantSemitones = 3.5;
                settings.noiseGateThresholdDb = -54.0;
                settings.compressorThresholdDb = -16.0;
                settings.compressorRatio = 2.8;
                settings.outputGainDb = 1.5;
                return settings;
            }()
        },
        {
            QStringLiteral("Deep Wolf"),
            QStringLiteral("Lower pitch with preserved formants and soft echo."),
            [] {
                EffectSettings settings;
                settings.pitchSemitones = -6.0;
                settings.formantSemitones = -3.0;
                settings.echoMix = 0.12;
                settings.echoDelayMs = 95.0;
                settings.outputGainDb = 2.0;
                return settings;
            }()
        },
        {
            QStringLiteral("Robot"),
            QStringLiteral("Mechanical voice with ring modulation and radio color."),
            [] {
                EffectSettings settings;
                settings.robotMix = 0.85;
                settings.robotFrequencyHz = 52.0;
                settings.radioMix = 0.35;
                settings.compressorThresholdDb = -15.0;
                settings.outputGainDb = 1.0;
                return settings;
            }()
        },
        {
            QStringLiteral("Radio"),
            QStringLiteral("Telephone-style band-limited voice."),
            [] {
                EffectSettings settings;
                settings.radioMix = 0.9;
                settings.noiseGateThresholdDb = -50.0;
                settings.compressorThresholdDb = -18.0;
                return settings;
            }()
        },
        {
            QStringLiteral("Monster"),
            QStringLiteral("Aggressive low voice with more air and body."),
            [] {
                EffectSettings settings;
                settings.pitchSemitones = -10.0;
                settings.formantSemitones = -4.0;
                settings.echoMix = 0.18;
                settings.echoDelayMs = 120.0;
                settings.compressorThresholdDb = -10.0;
                settings.compressorRatio = 4.5;
                settings.outputGainDb = 3.0;
                return settings;
            }()
        }
    };
}

int PresetModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_presets.size();
}

QVariant PresetModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_presets.size()) {
        return {};
    }

    const auto& preset = m_presets[index.row()];
    switch (role) {
    case NameRole:
        return preset.name;
    case DescriptionRole:
        return preset.description;
    case SettingsRole:
        return preset.settings.toVariantMap();
    default:
        return {};
    }
}

QHash<int, QByteArray> PresetModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {DescriptionRole, "description"},
        {SettingsRole, "settings"}
    };
}

QVariantMap PresetModel::preset(int index) const
{
    if (index < 0 || index >= m_presets.size()) {
        return {};
    }
    return m_presets[index].settings.toVariantMap();
}

QString PresetModel::presetName(int index) const
{
    if (index < 0 || index >= m_presets.size()) {
        return {};
    }
    return m_presets[index].name;
}

QString PresetModel::presetDescription(int index) const
{
    if (index < 0 || index >= m_presets.size()) {
        return {};
    }
    return m_presets[index].description;
}

} // namespace volchay
