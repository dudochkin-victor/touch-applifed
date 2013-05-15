#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#define UNIT_TEST

#include "configdata.h"
#include <tr1/memory>

using std::tr1::shared_ptr;

class Ut_ConfigData : public QObject
{
    Q_OBJECT

public:
    Ut_ConfigData();
    virtual ~Ut_ConfigData();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testDefaults();
    void testCPULoadThreshold();
    void testCPULoadPollingDelay();
    void testRePrestartDelay();
    void testRePrestartTrigger();
    void testAddPrestartApp();

private:
    shared_ptr<ConfigData> m_subject;
};
