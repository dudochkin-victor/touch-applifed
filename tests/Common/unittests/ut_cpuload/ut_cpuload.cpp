#include "ut_cpuload.h"

Ut_CPULoad::Ut_CPULoad()
{}

Ut_CPULoad::~Ut_CPULoad()
{}

void Ut_CPULoad::initTestCase()
{}

void Ut_CPULoad::cleanupTestCase()
{
    m_subject.reset();
}

void Ut_CPULoad::testInit()
{
    m_subject.reset(new CPULoad());
    QVERIFY(m_subject->getValue() == -1);
    m_subject->m_load = 100;
    m_subject->init();
    QVERIFY(m_subject->getValue() == -1);
}

void Ut_CPULoad::testForceValue()
{
    m_subject.reset(new CPULoad());
    QVERIFY(m_subject->getValue() == -1);
    m_subject->m_load = 100;
    QVERIFY(m_subject->getValue() == 100);
    m_subject->forceValue(-1);
    QVERIFY(m_subject->getValue() == 100);
    m_subject->forceValue(0);
    QVERIFY(m_subject->getValue() == 0);
    m_subject->forceValue(50);
    QVERIFY(m_subject->getValue() == 50);
    m_subject->m_load = 100;
    m_subject->forceValue(-1);
    QVERIFY(m_subject->getValue() == 100);
}

void Ut_CPULoad::testMeasurement()
{
    m_subject.reset(new CPULoad());
    m_subject->update();
    sleep(1);
    m_subject->update();
    QVERIFY(m_subject->getValue() > -1 && m_subject->getValue() <= 100);
}

void Ut_CPULoad::testMeasurementForceValue()
{
    m_subject.reset(new CPULoad());
    m_subject->forceValue(50);
    m_subject->update();
    sleep(1);
    m_subject->update();
    QVERIFY(m_subject->getValue() == 50);
}

QTEST_MAIN(Ut_CPULoad);
