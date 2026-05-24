#include "config/AppSettings.h"

#include "logging/Logger.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <spdlog/spdlog.h>

namespace volchay {
namespace {

QString settingsPath()
{
    const auto base = QCoreApplication::applicationDirPath().isEmpty()
        ? QDir::currentPath()
        : QCoreApplication::applicationDirPath();
    return QDir(base).filePath("config/settings.json");
}

QVariantMap jsonToVariantMap(const QJsonObject& object)
{
    QVariantMap map;
    for (auto it = object.begin(); it != object.end(); ++it) {
        map.insert(it.key(), it.value().toVariant());
    }
    return map;
}

QJsonObject variantMapToJson(const QVariantMap& map)
{
    QJsonObject object;
    for (auto it = map.begin(); it != map.end(); ++it) {
        object.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    return object;
}

} // namespace

QVariantMap AppSettings::toVariantMap() const
{
    return {
        {"inputDevice", inputDevice},
        {"outputDevice", outputDevice},
        {"sampleRate", sampleRate},
        {"framesPerBuffer", framesPerBuffer},
        {"effects", effects}
    };
}

AppSettings AppSettings::fromVariantMap(const QVariantMap& map)
{
    AppSettings settings;
    settings.inputDevice = map.value("inputDevice", settings.inputDevice).toInt();
    settings.outputDevice = map.value("outputDevice", settings.outputDevice).toInt();
    settings.sampleRate = map.value("sampleRate", settings.sampleRate).toInt();
    settings.framesPerBuffer = map.value("framesPerBuffer", settings.framesPerBuffer).toInt();
    settings.effects = map.value("effects").toMap();
    return settings;
}

AppSettings AppSettings::load()
{
    QFile file(settingsPath());
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return {};
    }

    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        spdlog::warn("Settings file is not a JSON object: {}", settingsPath().toStdString());
        return {};
    }

    return fromVariantMap(jsonToVariantMap(doc.object()));
}

bool AppSettings::save(const AppSettings& settings)
{
    const auto path = settingsPath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        spdlog::error("Failed to write settings file {}", path.toStdString());
        return false;
    }

    const auto doc = QJsonDocument::fromVariant(settings.toVariantMap());
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

} // namespace volchay
