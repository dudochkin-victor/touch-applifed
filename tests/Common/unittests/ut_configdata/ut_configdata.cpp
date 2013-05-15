#include "ut_configdata.h"

Ut_ConfigData::Ut_ConfigData()
{}

Ut_ConfigData::~Ut_ConfigData()
{}

void Ut_ConfigData::initTestCase()
{}

void Ut_ConfigData::cleanupTestCase()
{
    m_subject.reset();
}

void Ut_ConfigData::testDefaults()
{
    m_subject.reset(new ConfigData);
    QVERIFY(m_subject->defaultCPULoadThreshold()  == 25);
    QVERIFY(m_subject->cpuLoadPollingDelay()      == 1);
    QVERIFY(m_subject->rePrestartDelay()          == 3 * 3600);
    QVERIFY(m_subject->defaultRePrestartTrigger() == 3);
}

void Ut_ConfigData::testCPULoadThreshold()
{
    m_subject.reset(new ConfigData);
    m_subject->setDefaultCPULoadThreshold(50);
    QVERIFY(m_subject->defaultCPULoadThreshold() == 50);
}

void Ut_ConfigData::testCPULoadPollingDelay()
{
    m_subject.reset(new ConfigData);
    m_subject->setCPULoadPollingDelay(1);
    QVERIFY(m_subject->cpuLoadPollingDelay() == 1);
}

void Ut_ConfigData::testRePrestartDelay()
{
    m_subject.reset(new ConfigData);
    m_subject->setRePrestartDelay(1000);
    QVERIFY(m_subject->rePrestartDelay() == 1000);
}

void Ut_ConfigData::testRePrestartTrigger()
{
    m_subject.reset(new ConfigData);
    m_subject->setDefaultRePrestartTrigger(10);
    QVERIFY(m_subject->defaultRePrestartTrigger() == 10);
}

void Ut_ConfigData::testAddPrestartApp()
{
    m_subject.reset(new ConfigData);
    PrestartApp foo("foo", "foo", 0);
    m_subject->addPrestartApp(&foo);
    PrestartApp bar("bar", "bar", 0);
    m_subject->addPrestartApp(&bar);
    QVERIFY(m_subject->prestartApps().contains(&foo));
    QVERIFY(m_subject->prestartApps().contains(&bar));
}

QTEST_MAIN(Ut_ConfigData);
