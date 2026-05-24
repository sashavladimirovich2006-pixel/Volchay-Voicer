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
        QVERIFY(model.rowCount() >= 5);
        QVERIFY(!model.presetName(0).isEmpty());
        QVERIFY(!model.presetDescription(0).isEmpty());
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
