#include <QtTest/QtTest>
#include <QObject>

#define UNIT_TEST

#include "prestartapp.h"

class Ut_PrestartApp : public QObject
{
    Q_OBJECT

public:
    Ut_PrestartApp();
    virtual ~Ut_PrestartApp();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testGeneral();
    void testReleasedCount();
    void testSetRePrestarting();
    void testSetRePrestartTrigger();
    void testSetForcedRePrestartNotAllowed();
};
