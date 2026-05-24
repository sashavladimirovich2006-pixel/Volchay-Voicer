#include "app/AppInfo.h"

#include "audio/AudioDeviceModel.h"
#include "audio/AudioEngineController.h"
#include "config/AppSettings.h"
#include "logging/LogController.h"
#include "logging/Logger.h"
#include "presets/PresetModel.h"

#include <QGuiApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <spdlog/spdlog.h>

using namespace volchay;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(AppName);
    app.setApplicationDisplayName(AppName);
    app.setOrganizationName("Volchay");
    app.setOrganizationDomain("volchay.local");

    QQuickStyle::setStyle("Fusion");
    Logger::initialize();

    AudioDeviceModel inputDevices(AudioDeviceModel::Direction::Input);
    AudioDeviceModel outputDevices(AudioDeviceModel::Direction::Output);
    PresetModel presetModel;
    LogController logController;
    AudioEngineController audioController;

    const auto settings = AppSettings::load();
    audioController.restore(settings.toVariantMap());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("inputDevices", &inputDevices);
    engine.rootContext()->setContextProperty("outputDevices", &outputDevices);
    engine.rootContext()->setContextProperty("presetModel", &presetModel);
    engine.rootContext()->setContextProperty("logController", &logController);
    engine.rootContext()->setContextProperty("audioController", &audioController);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&] {
        const auto current = AppSettings::fromVariantMap(audioController.snapshot());
        AppSettings::save(current);
        Logger::flush();
    });

    engine.loadFromModule("VolchayVoicer", "Main");
    if (engine.rootObjects().isEmpty()) {
        spdlog::critical("Failed to load QML root");
        return -1;
    }

    return app.exec();
}
