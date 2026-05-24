#include "app/AppInfo.h"

#include "audio/AudioDeviceModel.h"
#include "audio/AudioEngineController.h"
#include "config/AppSettings.h"
#include "logging/LogController.h"
#include "logging/Logger.h"
#include "presets/PresetModel.h"

#include <QGuiApplication>
#include <QCoreApplication>
#include <QColor>
#include <QPalette>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <spdlog/spdlog.h>

using namespace volchay;

namespace {

QPalette makeDarkPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#0b0f14"));
    palette.setColor(QPalette::WindowText, QColor("#eef4fb"));
    palette.setColor(QPalette::Base, QColor("#111720"));
    palette.setColor(QPalette::AlternateBase, QColor("#161d27"));
    palette.setColor(QPalette::ToolTipBase, QColor("#eef4fb"));
    palette.setColor(QPalette::ToolTipText, QColor("#0b0f14"));
    palette.setColor(QPalette::Text, QColor("#dce5ef"));
    palette.setColor(QPalette::Button, QColor("#171d26"));
    palette.setColor(QPalette::ButtonText, QColor("#eef4fb"));
    palette.setColor(QPalette::BrightText, QColor("#ffffff"));
    palette.setColor(QPalette::Link, QColor("#5aa4ff"));
    palette.setColor(QPalette::Highlight, QColor("#2fd27f"));
    palette.setColor(QPalette::HighlightedText, QColor("#08130e"));
    palette.setColor(QPalette::PlaceholderText, QColor("#7d91a6"));
    palette.setColor(QPalette::Light, QColor("#2d3947"));
    palette.setColor(QPalette::Mid, QColor("#212a35"));
    palette.setColor(QPalette::Dark, QColor("#0f1319"));
    palette.setColor(QPalette::Shadow, QColor("#05070a"));
    return palette;
}

} // namespace

int main(int argc, char* argv[])
{
    QQuickStyle::setStyle("Fusion");
    QGuiApplication app(argc, argv);
    app.setApplicationName(AppName);
    app.setApplicationDisplayName(AppName);
    app.setOrganizationName("Volchay");
    app.setOrganizationDomain("volchay.local");
    app.setPalette(makeDarkPalette());
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
