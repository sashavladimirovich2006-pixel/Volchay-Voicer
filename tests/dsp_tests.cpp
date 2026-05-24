#include "dsp/BasicEffects.h"
#include "dsp/EffectSettings.h"

#include <QtTest/QtTest>

using namespace volchay;

class DspTests : public QObject {
    Q_OBJECT

private slots:
    void clampSampleKeepsRange()
    {
        QCOMPARE(clampSample(2.2f), 1.0f);
        QCOMPARE(clampSample(-2.2f), -1.0f);
    }

    void dbConversionIsMonotonic()
    {
        QVERIFY(dbToLinear(-6.0) < dbToLinear(0.0));
        QVERIFY(dbToLinear(0.0) < dbToLinear(6.0));
    }

    void settingsRoundTrip()
    {
        EffectSettings original;
        original.pitchSemitones = -4.5;
        original.echoMix = 0.2;
        original.outputGainDb = 3.0;

        const auto restored = EffectSettings::fromVariantMap(original.toVariantMap());
        QCOMPARE(restored.pitchSemitones, -4.5);
        QCOMPARE(restored.echoMix, 0.2);
        QCOMPARE(restored.outputGainDb, 3.0);
    }
};

int runDspTests(int argc, char** argv)
{
    DspTests tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "dsp_tests.moc"

