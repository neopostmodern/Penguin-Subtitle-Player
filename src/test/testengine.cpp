#include "testengine.h"
#include "../engine.h"

TestEngine::TestEngine() {}

void TestEngine::initTestCase() {
    QString content = R"(1
00:00:00,050 --> 00:00:30,000
Foo

2
00:01:00,300 --> 00:02:00,900  X1:40 X2:600 Y1:20 Y2:50
Foo
Bar

3
00:03:45,150 --> 01:59:59,990
<i>Italics</i><br><b>Bold</b><br><u>Underline</u><s>Strikeout</s>

)";
    QFile tmpFile(QDir::tempPath() + "/XXXXXX.srt");
    if (!tmpFile.open(QIODevice::WriteOnly)) {
        QFAIL("Cannot open temp file");
    }
    tmpFileName = tmpFile.fileName();

    QTextStream outStream(&tmpFile);
    outStream << content;
    tmpFile.close();
}

void TestEngine::testCurrentSubtitleIndex() {
    Engine engine = Engine(tmpFileName, "");

    for (int sliderMoved=0; sliderMoved<=1; sliderMoved++){
        QCOMPARE(engine.currentSubtitleIndex(0LL, sliderMoved), -1);      // before 0

        QCOMPARE(engine.currentSubtitleIndex(50LL, sliderMoved), 0);      // start of 0
        QCOMPARE(engine.currentSubtitleIndex(1000LL, sliderMoved), 0);    // inside 0
        QCOMPARE(engine.currentSubtitleIndex(30000LL, sliderMoved), 0);   // end of 0

        QCOMPARE(engine.currentSubtitleIndex(30001LL, sliderMoved), -2); // after 0 and before 1

        QCOMPARE(engine.currentSubtitleIndex(60300LL, sliderMoved), 1);   // start of 1
        QCOMPARE(engine.currentSubtitleIndex(70000LL, sliderMoved), 1);   // inside 1
        QCOMPARE(engine.currentSubtitleIndex(120900LL, sliderMoved), 1);  // end of 1

        QCOMPARE(engine.currentSubtitleIndex(130000LL, sliderMoved), -3); // after 1 and before 2

        QCOMPARE(engine.currentSubtitleIndex(225150LL, sliderMoved), 2);  // start of 2
        QCOMPARE(engine.currentSubtitleIndex(325150LL, sliderMoved), 2);  // inside 2
        QCOMPARE(engine.currentSubtitleIndex(7199990LL, sliderMoved), 2); // end of 2

        QCOMPARE(engine.currentSubtitleIndex(9199990LL, sliderMoved), 2); // greater than end
    }
}

void TestEngine::testGetFinishTime() {
    Engine engine = Engine(tmpFileName, "");
    QCOMPARE(engine.getFinishTime(), 7199990LL);
}

void TestEngine::testGetTimeWithSubtitleOffset() {
    Engine engine = Engine(tmpFileName, "");

    QCOMPARE(engine.getTimeWithSubtitleOffset(50LL, 1), 60300LL);  // 0 + 1 = 1
    QCOMPARE(engine.getTimeWithSubtitleOffset(50LL, 2), 225150LL); // 0 + 2 = 2
    QCOMPARE(engine.getTimeWithSubtitleOffset(50LL, 3),
             7199990LL); // greater than end
    QCOMPARE(engine.getTimeWithSubtitleOffset(0LL, 1), 50LL); // -1 + 1 = 0

    QCOMPARE(engine.getTimeWithSubtitleOffset(7199990LL, -2),
             50LL); // 2 - 2 = 0
    QCOMPARE(engine.getTimeWithSubtitleOffset(7199990LL, -3),
             0LL); // before start

    QCOMPARE(engine.getTimeWithSubtitleOffset(0LL, -1), 0LL);  // -1 - 1 = -1
    QCOMPARE(engine.getTimeWithSubtitleOffset(50LL, -1), 0LL); // 0 - 1 = -1

    QCOMPARE(engine.getTimeWithSubtitleOffset(130000LL, 1), 225150LL); // between 1 and 2, +1 = 2
    QCOMPARE(engine.getTimeWithSubtitleOffset(130000LL, -1), 60300LL); // between 1 and 2, -1 = 1
}

void TestEngine::cleanupTestCase() {
    QFile tmpFile(tmpFileName);
    tmpFile.remove();
}
