#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#define UNIT_TEST

#include "cpuload.h"
#include <tr1/memory>

using std::tr1::shared_ptr;

class Ut_CPULoad : public QObject
{
    Q_OBJECT

public:
    Ut_CPULoad();
    virtual ~Ut_CPULoad();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testInit();
    void testForceValue();
    void testMeasurement();
    void testMeasurementForceValue();

private:
    shared_ptr<CPULoad> m_subject;
};
