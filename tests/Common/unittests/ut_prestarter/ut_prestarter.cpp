#include "ut_prestarter.h"

Ut_Prestarter::Ut_Prestarter()
{}

Ut_Prestarter::~Ut_Prestarter()
{}

void Ut_Prestarter::initTestCase()
{}

void Ut_Prestarter::cleanupTestCase()
{
    m_subject.reset();
}

void Ut_Prestarter::testAddPrestartApp()
{
    m_subject.reset(new Prestarter());
    PrestartApp * foo = new PrestartApp("foo1", "com.nokia.foo1", 0);

    m_subject->addPrestartApp(foo);

    QVERIFY( m_subject->findAppWithService(foo->service()));
    QVERIFY(!m_subject->findAppWithService("com.nokia.dummy12345"));

    QVERIFY(m_subject->m_vectApps.size() == 1);

    // Test that the same application is not added to the list multiple times
    int sizeBeforeSameAdd = m_subject->m_vectApps.size();
    m_subject->addPrestartApp(foo);

    QVERIFY(m_subject->m_vectApps.size() == sizeBeforeSameAdd);
}

void Ut_Prestarter::testReleasedPrestart()
{
    m_subject.reset(new Prestarter());
    m_subject->addPrestartApp(new PrestartApp("foo1", "com.nokia.foo1", 0));
    m_subject->addPrestartApp(new PrestartApp("foo2", "com.nokia.foo2", 0));

    foreach (PrestartApp * p, m_subject->m_vectApps)
        p->setReleased(false);

    m_subject->releasedPrestart(0);

    foreach (PrestartApp * p, m_subject->m_vectApps)
        QVERIFY(p->released());
}

// Test that Prestarter sets PrestartApp::released() correctly
void Ut_Prestarter::testRestoredPrestart()
{
    // Add some apps
    m_subject.reset(new Prestarter());
    m_subject->addPrestartApp(new PrestartApp("foo1", "com.nokia.foo1", 0));
    m_subject->addPrestartApp(new PrestartApp("foo2", "com.nokia.foo2", 0));

    // Set apps released
    foreach (PrestartApp * p, m_subject->m_vectApps)
        p->setReleased(true);

    // Tell Prestarter that all apps returned to the prestarted state
    m_subject->restoredPrestart(0);

    foreach (PrestartApp * p, m_subject->m_vectApps)
        QVERIFY(!p->released());
}

// Test that apps won't get re-prestarted if count is too low
void Ut_Prestarter::testRePrestart2Neg()
{
    m_subject.reset(new Prestarter());

    // Add some apps
    m_subject->addPrestartApp(new PrestartApp("foo1", "com.nokia.foo1", 0));
    m_subject->addPrestartApp(new PrestartApp("foo2", "com.nokia.foo2", 0));

    // Toggle setReleased to increase release count
    foreach (PrestartApp * p, m_subject->m_vectApps)
    {
        p->setRePrestartTrigger(1);
        p->setReleased(false);
        p->setReleased(true);
    }

    // Tell Prestarter that all apps returned to the prestarted state
    m_subject->restoredPrestart(0);

    foreach (PrestartApp * p, m_subject->m_vectApps)
    {
        QVERIFY(!p->released());
        QVERIFY(!p->rePrestarting());
    }
}

// Test that apps will get re-prestarted if count is high enough
// Test also that SIGTERM and SIGKILL gets send.
void Ut_Prestarter::testRePrestart2Pos()
{
    m_subject.reset(new Prestarter());

    // Add app
    PrestartApp * p = new PrestartApp("foo1", "com.nokia.foo1", 0, true);
    m_subject->addPrestartApp(p);

    // Toggle setReleased to increase release count
    p->setRePrestartTrigger(1);
    p->setReleased(false);
    p->setReleased(true);
    p->setReleased(false);
    p->setReleased(true);

    // Tell Prestarter that all apps returned to the prestarted state
    m_subject->restoredPrestart(0);

    QVERIFY(!p->released());
    QVERIFY(!p->sigTermSent());
    QVERIFY(p->rePrestarting());
}

// Test that apps won't get re-prestarted if denied
void Ut_Prestarter::testRePrestart3()
{
    m_subject.reset(new Prestarter());

    // Add app
    PrestartApp * p = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(p);

    // Toggle setReleased to increase release count
    p->setForcedRePrestartNotAllowed(true);
    p->setRePrestartTrigger(1);
    p->setReleased(false);
    p->setReleased(true);
    p->setReleased(false);
    p->setReleased(true);
    p->setReleased(false);
    p->setReleased(true);

    // Tell Prestarter that all apps returned to the prestarted state
    m_subject->restoredPrestart(0);

    QVERIFY(!p->released());
    QVERIFY(!p->rePrestarting());
}

// Test that app get SIGTERM and SIGKILL
void Ut_Prestarter::testRePrestartSig1()
{
    m_subject.reset(new Prestarter());

    // Add app
    PrestartApp * p = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(p);

    p->setRePrestarting(true);

    QVERIFY(!p->sigTermSent());
    QVERIFY(!p->sigKillSent());

    m_subject->triggerForcedRePrestarts();

    QVERIFY(p->sigTermSent());
    QVERIFY(!p->sigKillSent());

    m_subject->triggerForcedRePrestarts();

    QVERIFY(p->sigTermSent());
    QVERIFY(p->sigKillSent());
}

// Test that app get SIGTERM, but not SIGKILL
void Ut_Prestarter::testRePrestartSig2()
{
    m_subject.reset(new Prestarter());

    // Add app
    PrestartApp * p = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(p);

    p->setRePrestarting(true);

    QVERIFY(!p->sigTermSent());
    QVERIFY(!p->sigKillSent());

    m_subject->triggerForcedRePrestarts();

    QVERIFY(p->sigTermSent());
    QVERIFY(!p->sigKillSent());

    p->setRePrestarting(false);

    m_subject->triggerForcedRePrestarts();

    QVERIFY(!p->sigTermSent());
    QVERIFY(!p->sigKillSent());
}

// Test that no signals are sent if app gets released from the prestarted state
void Ut_Prestarter::testRePrestartSig3()
{
    m_subject.reset(new Prestarter());

    // Add app
    PrestartApp * p = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(p);

    p->setRePrestarting(true);

    QVERIFY(!p->sigTermSent());
    QVERIFY(!p->sigKillSent());

    p->setReleased(true);

    m_subject->triggerForcedRePrestarts();

    QVERIFY(!p->sigTermSent());
    QVERIFY(!p->sigKillSent());
}

void Ut_Prestarter::testSomethingToRePrestart()
{
    m_subject.reset(new Prestarter());

    QVERIFY(!m_subject->somethingToRePrestart());

    PrestartApp * app = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(app);

    QVERIFY(!m_subject->somethingToRePrestart());

    app->setRePrestarting(true);

    QVERIFY(m_subject->somethingToRePrestart());
}

void Ut_Prestarter::testDoReadConfigurationGlobals()
{
    m_subject.reset(new Prestarter());

    QString data("<?xml version=\"1.0\"?> \
                 <applifed default_cpu_load_threshold=\"50\" \
                 cpu_load_polling_delay=\"2\" \
                 re-prestart_delay=\"3600\" \
                 default_re-prestart_trigger=\"5\"> \
                 </applifed>");

    QXmlInputSource input;
    input.setData(data);
    m_subject->doReadConfiguration(input);

    // Test that global default settings are correct
    QVERIFY(m_subject->m_cpuLoadPollingDelay == 2);
    QVERIFY(m_subject->m_rePrestartDelay     == 3600);

    data = "<?xml version=\"1.0\"?> \
            <applifed default_cpu_load_threshold=\"51\" \
            cpu_load_polling_delay=\"3\" \
            re-prestart_delay=\"3601\" \
            default_re-prestart_trigger=\"6\"> \
            </applifed>";

    input.setData(data);

    m_subject->doReadConfiguration(input);

    // Test that global default settings are correct
    QVERIFY(m_subject->m_cpuLoadPollingDelay == 3);
    QVERIFY(m_subject->m_rePrestartDelay     == 3601);
}

void Ut_Prestarter::testDoReadConfigurationApplications()
{
    m_subject.reset(new Prestarter());

    QString data("<?xml version=\"1.0\"?> \
                  <applifed> \
                  <application name=\"bar1\" service=\"com.foo.bar1\" priority=\"4\"/> \
                  <application name=\"bar2\" service=\"com.foo.bar2\" priority=\"3\"/> \
                  <application name=\"bar3\" service=\"com.foo.bar3\" priority=\"2\"/> \
                  <application name=\"bar4\" service=\"com.foo.bar4\" priority=\"1\"/> \
                  </applifed>");

    QXmlInputSource input;
    input.setData(data);
    m_subject->doReadConfiguration(input);

    // Test that added apps are there
    QVERIFY(m_subject->m_vectApps.size() == 4);

    PrestartApp * app = m_subject->findAppWithService("com.foo.bar1");
    QVERIFY(app);

    app = m_subject->findAppWithService("com.foo.bar2");
    QVERIFY(app);

    app = m_subject->findAppWithService("com.foo.bar3");
    QVERIFY(app);

    app = m_subject->findAppWithService("com.foo.bar4");
    QVERIFY(app);

    // This should not be there
    app = m_subject->findAppWithService("com.foo.bar5");
    QVERIFY(!app);
}

void Ut_Prestarter::testDoReadConfigurationApplicationParams1()
{
    m_subject.reset(new Prestarter());

    QString data("<?xml version=\"1.0\"?> \
                  <applifed> \
                  <application name=\"bar1\" service=\"com.foo.bar1\" \
                  priority=\"4\" \
                  cpu_load_threshold=\"10\" \
                  forced_re-prestart_not_allowed=\"1\" \
                  re-prestart_trigger=\"3\" \
                  /> \
                  <application name=\"bar2\" service=\"com.foo.bar2\" \
                  priority=\"5\" \
                  cpu_load_threshold=\"11\" \
                  forced_re-prestart_not_allowed=\"0\" \
                  re-prestart_trigger=\"4\" \
                  /> \
                  </applifed>");

    QXmlInputSource input;
    input.setData(data);
    m_subject->doReadConfiguration(input);

    // Test that params are correctly set
    PrestartApp * app = m_subject->findAppWithService("com.foo.bar1");
    QVERIFY(app);
    QVERIFY(app->name() == "bar1");
    QVERIFY(app->priority() == 4);
    QVERIFY(app->cpuLoadThreshold() == 10);
    QVERIFY(app->forcedRePrestartNotAllowed() == true);
    QVERIFY(app->rePrestartTrigger() == 3);

    app = m_subject->findAppWithService("com.foo.bar2");
    QVERIFY(app);
    QVERIFY(app->name() == "bar2");
    QVERIFY(app->priority() == 5);
    QVERIFY(app->cpuLoadThreshold() == 11);
    QVERIFY(app->forcedRePrestartNotAllowed() == false);
    QVERIFY(app->rePrestartTrigger() == 4);
}

void Ut_Prestarter::testDoReadConfigurationApplicationParams2()
{
    m_subject.reset(new Prestarter());

    QString data("<?xml version=\"1.0\"?> \
                  <applifed \
                  default_cpu_load_threshold=\"51\" \
                  default_re-prestart_trigger=\"6\"> \
                  <application name=\"bar1\" service=\"com.foo.bar1\" \
                  cpu_load_threshold=\"10\" \
                  re-prestart_trigger=\"3\"/> \
                  <application name=\"bar2\" service=\"com.foo.bar2\"/> \
                  </applifed>");

    QXmlInputSource input;
    input.setData(data);
    m_subject->doReadConfiguration(input);

    // Test that default settings are used correctly
    PrestartApp * app = m_subject->findAppWithService("com.foo.bar1");
    QVERIFY(app);
    QVERIFY(app->cpuLoadThreshold() == 10);
    QVERIFY(app->rePrestartTrigger() == 3);

    app = m_subject->findAppWithService("com.foo.bar2");
    QVERIFY(app);
    QVERIFY(app->cpuLoadThreshold() == 51);
    QVERIFY(app->rePrestartTrigger() == 6);
}

void Ut_Prestarter::testDoReadConfigurationAllowLazyShutdown()
{
    m_subject.reset(new Prestarter());

    QString data("<?xml version=\"1.0\"?> \
                  <applifed> \
                  <allow_lazy_shutdown binary_path=\"/usr/bin/foo1\"/> \
                  <allow_lazy_shutdown binary_path=\"/usr/bin/foo2\"/> \
                  </applifed>");

    QXmlInputSource input;
    input.setData(data);
    m_subject->doReadConfiguration(input);

    QVERIFY(m_subject->isLazyShutdownAllowed("/usr/bin/foo1"));
    QVERIFY(m_subject->isLazyShutdownAllowed("/usr/bin/foo2"));
    QVERIFY(!m_subject->isLazyShutdownAllowed("/usr/bin/foo3"));
}

void Ut_Prestarter::testKillPrestartedApplications()
{
    m_subject.reset(new Prestarter());

    // Add apps
    PrestartApp * p1 = new PrestartApp("foo1", "com.nokia.foo1", 0);
    m_subject->addPrestartApp(p1);

    PrestartApp * p2 = new PrestartApp("foo1", "com.nokia.foo2", 0);
    m_subject->addPrestartApp(p2);

    QVERIFY(!p1->sigTermSent());
    QVERIFY(!p1->sigKillSent());
    QVERIFY(!p2->sigTermSent());
    QVERIFY(!p2->sigKillSent());

    // Set only p1 released
    p1->setReleased(true);

    m_subject->killPrestartedApplications();

    QVERIFY(p1->sigTermSent());
    QVERIFY(!p1->sigKillSent());
    QVERIFY(p2->sigTermSent());
    QVERIFY(!p2->sigKillSent());
}

QTEST_MAIN(Ut_Prestarter);
