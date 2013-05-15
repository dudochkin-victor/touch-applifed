#include "ut_prestartapp.h"

Ut_PrestartApp::Ut_PrestartApp()
{}

Ut_PrestartApp::~Ut_PrestartApp()
{}

void Ut_PrestartApp::initTestCase()
{}

void Ut_PrestartApp::cleanupTestCase()
{}

void Ut_PrestartApp::testGeneral()
{
    PrestartApp subject("foo", "com.foo.bar", 0);

    QVERIFY(subject.name() == "foo");
    QVERIFY(subject.service() == "com.foo.bar");

    QVERIFY(subject.testMode() == false);

    QVERIFY(subject.priority() == 0);
    subject.setPriority(1);
    QVERIFY(subject.priority() == 1);

    QVERIFY(subject.pid() == 0);
    subject.setPid(1);
    QVERIFY(subject.pid() == 1);

    QVERIFY(subject.cpuLoadThreshold() == 0);
    subject.setCPULoadThreshold(50);
    QVERIFY(subject.cpuLoadThreshold() == 50);

    QVERIFY(subject.prestarted() == false);
    subject.setPrestarted(true);
    QVERIFY(subject.prestarted() == true);
    subject.setPrestarted(false);
    QVERIFY(subject.prestarted() == false);
}

void Ut_PrestartApp::testReleasedCount()
{
    PrestartApp subject("foo", "com.foo.bar", 0);

    subject.setReleased(false);
    QVERIFY(subject.releasedCount() == 0);
    subject.setReleased(true);
    QVERIFY(subject.releasedCount() == 1);
    subject.setReleased(false);
    QVERIFY(subject.releasedCount() == 1);
    subject.setReleased(true);
    QVERIFY(subject.releasedCount() == 2);
    subject.setReleased(true);
    QVERIFY(subject.releasedCount() == 2);
    subject.resetReleasedCount();
    QVERIFY(subject.releasedCount() == 0);

    subject.setReleased(false);
    QVERIFY(subject.releasedCount() == 0);
    subject.setReleased(true);
    QVERIFY(subject.releasedCount() == 1);
    subject.setPrestarted(false);
    QVERIFY(subject.releasedCount() == 0);
}

void Ut_PrestartApp::testSetRePrestarting()
{
    PrestartApp subject("foo", "com.foo.bar", 0);

    QVERIFY(subject.rePrestarting() == false);

    subject.setSigTermSent(true);
    subject.setSigKillSent(true);
    subject.setRePrestarting(true);

    QVERIFY(subject.rePrestarting() == true);

    subject.setRePrestarting(false);

    QVERIFY(subject.rePrestarting() == false);
    QVERIFY(subject.sigTermSent() == false);
    QVERIFY(subject.sigKillSent() == false);

    subject.setRePrestarting(true);
    QVERIFY(subject.rePrestarting() == true);

    subject.setRePrestarting(true);
    QVERIFY(subject.rePrestarting() == true);
    subject.setPrestarted(false);
    QVERIFY(subject.rePrestarting() == false);
}

void Ut_PrestartApp::testSetRePrestartTrigger()
{
    PrestartApp subject("foo", "com.foo.bar", 0);

    subject.setRePrestartTrigger(1);
    QVERIFY(subject.rePrestartTrigger() == 1);
    subject.setRePrestartTrigger(2);
    QVERIFY(subject.rePrestartTrigger() == 2);
    subject.setRePrestartTrigger(3);
    QVERIFY(subject.rePrestartTrigger() == 3);
    subject.setRePrestartTrigger(0);
    QVERIFY(subject.rePrestartTrigger() == 0);
}

void Ut_PrestartApp::testSetForcedRePrestartNotAllowed()
{
    PrestartApp subject("foo", "com.foo.bar", 0);

    QVERIFY(subject.forcedRePrestartNotAllowed() == false);
    subject.setForcedRePrestartNotAllowed(true);
    QVERIFY(subject.forcedRePrestartNotAllowed() == true);
    subject.setForcedRePrestartNotAllowed(false);
    QVERIFY(subject.forcedRePrestartNotAllowed() == false);
}

QTEST_MAIN(Ut_PrestartApp);
