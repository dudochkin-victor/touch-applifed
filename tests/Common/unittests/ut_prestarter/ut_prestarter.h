#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#define UNIT_TEST

#include "prestarter.h"
#include "prestartapp.h"

#include <tr1/memory>

using std::tr1::shared_ptr;

class Ut_Prestarter : public QObject
{
    Q_OBJECT

public:
    Ut_Prestarter();
    virtual ~Ut_Prestarter();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testAddPrestartApp();
    void testRestoredPrestart();
    void testRePrestart2Neg();
    void testRePrestart2Pos();
    void testRePrestart3();
    void testRePrestartSig1();
    void testRePrestartSig2();
    void testRePrestartSig3();
    void testReleasedPrestart();
    void testSomethingToRePrestart();
    void testDoReadConfigurationGlobals();
    void testDoReadConfigurationApplications();
    void testDoReadConfigurationApplicationParams1();
    void testDoReadConfigurationApplicationParams2();
    void testDoReadConfigurationAllowLazyShutdown();
    void testKillPrestartedApplications();

private:
    shared_ptr<Prestarter> m_subject;
};
