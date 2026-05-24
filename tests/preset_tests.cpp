#include "presets/PresetModel.h"

#include <QtTest/QtTest>
#include <QCoreApplication>

using namespace volchay;

class PresetTests : public QObject {
    Q_OBJECT

private slots:
    void modelHasPresets()
    {
        PresetModel model;
        QVERIFY(model.rowCount() >= 6);
        QVERIFY(!model.presetName(0).isEmpty());
        QVERIFY(!model.presetDescription(0).isEmpty());
    }

    void hasFemaleVoicePreset()
    {
        PresetModel model;

        bool found = false;
        for (int row = 0; row < model.rowCount(); ++row) {
            if (model.presetName(row) == QStringLiteral("Женский голос")) {
                const auto preset = model.preset(row);
                QCOMPARE(preset.value("pitchSemitones").toDouble(), 5.0);
                QCOMPARE(preset.value("formantSemitones").toDouble(), 3.5);
                QCOMPARE(preset.value("compressorThresholdDb").toDouble(), -16.0);
                QCOMPARE(preset.value("outputGainDb").toDouble(), 1.5);
                found = true;
                break;
            }
        }

        QVERIFY(found);
    }
};

int runDspTests(int argc, char** argv);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    auto status = runDspTests(argc, argv);
    PresetTests tests;
    return status | QTest::qExec(&tests, argc, argv);
}

#include "preset_tests.moc"
