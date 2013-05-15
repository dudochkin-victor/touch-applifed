/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of applifed.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "prestarter.h"
#include "systemstate.h"
#include "configparser.h"
#include "logger.h"

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <signal.h>
#include <errno.h>

#include "x11util.h"

namespace
{
    // One second in ms
    const unsigned int ONE_SECOND = 1000;
}

Prestarter::Prestarter(bool testMode) :
    m_testMode(testMode),
    m_connection(QDBusConnection::sessionBus()),
    m_serviceWatcher(),
    m_systemState(new SystemState),
    m_cpuLoadPollingEnabled(false),
    m_state(Init),
    m_load(100),
    m_cpuLoadPollingDelay(0), // Can be overridden in the conf file
    m_rePrestartDelay(0),     // Can be overridden in the conf file
    m_forcedRePrestartTimer()
{
    // Attach serviceWatcher to the session bus
    m_serviceWatcher.setConnection(m_connection);

    // Set watcher to watch all types of events
    m_serviceWatcher.setWatchMode(QDBusServiceWatcher::WatchForRegistration   |
                                  QDBusServiceWatcher::WatchForUnregistration |
                                  QDBusServiceWatcher::WatchForOwnerChange);

    connect(&m_serviceWatcher, SIGNAL(serviceRegistered(const QString &)),
            this, SLOT(handleRegisteredService(const QString &)));

    connect(&m_serviceWatcher, SIGNAL(serviceUnregistered(const QString &)),
            this, SLOT(handleUnregisteredService(const QString &)));

    connect(&m_serviceWatcher, SIGNAL(serviceOwnerChanged(const QString &, const QString &, const QString &)),
            this, SLOT(handleServiceOwnerChange(const QString &, const QString &, const QString &)));

    connect(&m_forcedRePrestartTimer, SIGNAL(timeout()), this, SLOT(triggerForcedRePrestarts()));

    m_forcedRePrestartTimer.setSingleShot(true);
}

void Prestarter::handleScreenBlanked(bool state)
{
    if (state)
    {
        if (!m_forcedRePrestartTimer.isActive())
        {
            if (somethingToRePrestart())
            {
                if (m_testMode)
                {
                    m_forcedRePrestartTimer.setInterval(ONE_SECOND);
                }
                else
                {
                    m_forcedRePrestartTimer.setInterval(ONE_SECOND * m_rePrestartDelay);
                }

                m_forcedRePrestartTimer.start();

                Logger::logNotice("Re-prestart timer (%d ms) started", m_forcedRePrestartTimer.interval());
            }
        }
    }
    else
    {
        if (m_forcedRePrestartTimer.isActive())
        {
            Logger::logNotice("Re-prestart timer stopped");

            m_forcedRePrestartTimer.stop();
        }
    }
}

// This method gets triggered by:
// A) Initing the Prestarter
// B) A new CPU load value is signaled
// C) A watched service unregisters from the session bus
void Prestarter::runState()
{
    switch (m_state)
    {
    default:
    case Init:

        // Init stuff
        stateInit();
        break;

    case WaitForPrestart:

        // Wait for something to be prestarted
        stateWaitForPrestart();
        break;

    case WaitForCPU:

        // Wait that cpu usage drops below the limit and prestart
        stateWaitForCPU();
        break;
    }
}

void Prestarter::stateInit()
{
    // Loop through all prestartable apps and push to stack
    Q_FOREACH(PrestartApp * app, m_vectApps)
    {
        pushApplicationToPrestartQueue(app);
    }

    setState(WaitForPrestart);
    runState();
}

void Prestarter::setState(State newState)
{
    m_state = newState;
}

void Prestarter::stateWaitForPrestart()
{
    if (m_queue.size())
    {
        startCPULoadPolling();
        setState(WaitForCPU);
    }
}

void Prestarter::stateWaitForCPU()
{
    if (m_queue.size())
    {
        const int th = m_queue.top()->cpuLoadThreshold();
        if (m_load < th)
        {
            Logger::logNotice("CPU load (%d) dropped below threshold (%d)",
                              m_load, th);

            // Take app from the stack
            PrestartApp * const app(m_queue.top());
            m_queue.pop();

            Logger::logNotice("Prestarting '%s', priority=%d",
                              app->service().toAscii().data(),
                              app->priority());

            // Try to prestart the application via D-Bus
            sendPrestartRequest(app);
        }
    }
    else
    {
        stopCPULoadPolling();
        setState(WaitForPrestart);
    }
}

void Prestarter::startCPULoadPolling()
{
    if (!m_cpuLoadPollingEnabled)
    {
        // Connect CPU load updates
        connect(m_systemState.get(), SIGNAL(cpuLoadUpdated(int)), this, SLOT(updateCPULoad(int)));
        m_systemState->enableCPULoadPolling(true, m_cpuLoadPollingDelay);
        m_cpuLoadPollingEnabled = true;
    }
}

void Prestarter::stopCPULoadPolling()
{
    if (m_cpuLoadPollingEnabled)
    {
        // Disconnect CPU load updates
        disconnect(m_systemState.get(), SIGNAL(cpuLoadUpdated(int)), this, SLOT(updateCPULoad(int)));
        m_systemState->enableCPULoadPolling(false);
        m_cpuLoadPollingEnabled = false;
    }
}

bool Prestarter::somethingToRePrestart() const
{
    Q_FOREACH(PrestartApp * app, m_vectApps)
    {
        if (app->rePrestarting())
        {
            return true;
        }
    }

    return false;
}

void Prestarter::handleRegisteredService(const QString & service)
{
    // Mark the app as prestarted
    PrestartApp * app = findAppWithService(service);
    if (app && !app->prestarted())
    {
        // Get PID for the service
        app->setPid(m_connection.interface()->servicePid(service));
        app->setPrestarted(true);

        Logger::logNotice("'%s' got registered with pid '%d'..",
                          service.toStdString().c_str(),
                          app->pid());
    }
}

void Prestarter::handleUnregisteredService(const QString & service)
{
    // Check if the unregistered service was one the prestarted applications.
    // If so, then prestart it again..

    PrestartApp * app = findAppWithService(service);
    if (app && app->prestarted())
    {
        Logger::logNotice("'%s' got unregistered..", app->service().toAscii().data());
        app->setReleased(false);

        if (app->rePrestarting() || app->releasedCount() > 0)
        {
            // Re-prestart the terminated application
            Logger::logNotice("Re-prestarting '%s'..", app->service().toAscii().data());
            pushApplicationToPrestartQueue(app);
        }
        else
        {
            Logger::logNotice("Not re-prestarting '%s' because it was never released",
                              app->service().toAscii().data());
        }

        // Set app as not prestarted at this point, because this call resets
        // some variables
        app->setPrestarted(false);

        // Update state machine because this can cause a re-prestart.
        runState();
    }
}

void Prestarter::handleServiceOwnerChange(const QString & service, const QString & oldName, const QString & newName)
{
    // Qt (D-Bus?) might send serviceOwnerChanged instead of serviceUnregistered
    // and serviceRegistered:
    // - oldName == "" means a registered service
    // - newName == "" means an unregistered service

    if (newName == "")
    {
        handleUnregisteredService(service);
    }
    else if (oldName == "")
    {
        handleRegisteredService(service);
    }
}

void Prestarter::updateCPULoad(int cpuLoad)
{
    m_load = cpuLoad;

    // Update state machine because this can cause a prestart.
    runState();
}

bool Prestarter::readConfiguration(const QString & configPath)
{
    // Instantiate a QFile for the config file
    QFile xmlFile(configPath);
    if (xmlFile.exists())
    {
        // Set QXmlInputSource to the QFile
        return doReadConfiguration(QXmlInputSource(&xmlFile));
    }
    else
    {
        // Prestart conf dir didn't exist: exit
        Logger::logError("Prestart configuration file '%s' doesn't exist!", configPath.toAscii().data());
        return false;
    }

    return false;
}

bool Prestarter::doReadConfiguration(const QXmlInputSource & source)
{
    // Instantiate a simple (SAX2) reader
    QXmlSimpleReader reader;

    // Instantiate the parser/handler
    ConfigParser handler;

    // Set it to use the ConfigParser as the handler
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    // Parse the texture file
    if (reader.parse(source))
    {
        // Fetch the data
        Q_FOREACH(PrestartApp * app, handler.data().prestartApps())
        {
            addPrestartApp(app);
        }

        // Set CPU-load polling delay
        m_cpuLoadPollingDelay = handler.data().cpuLoadPollingDelay();

        // Set re-prestart delay
        m_rePrestartDelay = handler.data().rePrestartDelay();

        // Get external apps that are allowed to lazily shutdown
        m_allowedLazyShutdownApps = handler.data().allowedLazyShutdownApps();

        return true;
    }

    return false;
}

void Prestarter::addPrestartApp(PrestartApp * p)
{
    // Check that app is not already added
    if (p && !findAppWithService(p->service()))
    {
        // Add to app vector
        m_vectApps.append(p);

        // Check if the service was already registered (someone else
        // has already started it for some reason)
        if (m_connection.interface()->isServiceRegistered(p->service()))
        {
            // Get PID for the service
            p->setPid(m_connection.interface()->servicePid(p->service()));
            p->setPrestarted(true);
            p->setReleased(false);

            // Find out if there are windows mapped for that pid
            if (!X11Util::windowsForPid(p->pid()).isEmpty())
            {
                p->setReleased(true);
            }

            Logger::logNotice("'%s' was already registered with pid %d",
                              p->service().toStdString().c_str(),
                              p->pid());
        }

        // Add service to the services that are being watched by D-Bus watcher
        m_serviceWatcher.addWatchedService(p->service());
    }
}

bool Prestarter::isLazyShutdownAllowed(const QString & path) const
{
    return m_allowedLazyShutdownApps.contains(path);
}

void Prestarter::startPrestartActions()
{
    setState(Init);
    runState();
}

PrestartApp * Prestarter::findAppWithService(const QString & service) const
{
    Q_FOREACH(PrestartApp * app, m_vectApps)
    {
        if (app->service() == service)
        {
            return app;
        }
    }

    return NULL;
}

PrestartApp * Prestarter::findAppWithPid(unsigned int pid) const
{
    Q_FOREACH(PrestartApp * app, m_vectApps)
    {
        if (app->pid() == pid)
        {
            return app;
        }
    }

    return NULL;
}

void Prestarter::restoredPrestart(unsigned int pid)
{
    // Try to find released application with the given PID. If found,
    // set its state to restored and check if forced re-prestart is needed.
    Q_FOREACH(PrestartApp * p, m_vectApps)
    {
        if (p->released())
        {
            if (!pid || p->pid() == pid)
            {
                // Set the app not-released
                p->setReleased(false);
                Logger::logNotice("Application '%s' returned to prestarted state",
                                  p->service().toAscii().data());

                // Check if the app needs to be re-prestarted
                if (!p->forcedRePrestartNotAllowed())
                {
                    if (p->releasedCount() > p->rePrestartTrigger())
                    {
                        Logger::logNotice("Application '%s' scheduled for re-prestart",
                                          p->service().toAscii().data());
                        p->setRePrestarting(true);
                    }
                }
            }
        }
    }
}

void Prestarter::triggerForcedRePrestarts()
{
    Logger::logNotice("Forced re-prestarts triggered..");

    // Terminate applications that are scheduled for re-prestart
    Q_FOREACH(PrestartApp * p, m_vectApps)
    {
        // Check that app is scheduled for re-prestart
        if (p->rePrestarting())
        {
            // Check that app not currently released
            if (!p->released())
            {
                // Kill the application
                killApp(p);
            }
        }
    }
}

void Prestarter::killApp(PrestartApp * p)
{
    int signal = 0;

    // Check if SIGTERM was already tried
    if (!p->sigTermSent())
    {
        signal = SIGTERM;
        p->setSigTermSent(true);
    }
    else
    {
        signal = SIGKILL;
        p->setSigKillSent(true);
    }

    // Call kill and handle errors. Zero pid's are discarded mainly
    // for unit testability.
    if (p->pid())
    {
        Logger::logNotice("Sending signal %d to application '%s', pid %d..",
                          signal,
                          p->service().toAscii().data(),
                          p->pid());

        if (kill(p->pid(), signal) == -1)
        {
            if (errno == EPERM)
            {
                Logger::logError("No permissions to send signal %d to %d", signal, p->pid());
            }
            else if (errno == ESRCH)
            {
                Logger::logError("Pid %d doesn't exist or may be a zombie", p->pid());
                p->setRePrestarting(false);
            }
        }
        else
        {
            Logger::logNotice("Sending %d succeeded.", signal);
        }
    }
}

void Prestarter::releasedPrestart(unsigned int pid)
{
    // Try to find prestared application with the given PID
    Q_FOREACH(PrestartApp * p, m_vectApps)
    {
        if (!p->released())
        {
            if (!pid || p->pid() == pid)
            {
                p->setReleased(true);
                Logger::logNotice("Application '%s' released from prestarted state, count=%d",
                                  p->service().toAscii().data(),
                                  p->releasedCount());
            }
        }
    }
}

void Prestarter::pushApplicationToPrestartQueue(PrestartApp * app)
{
    Logger::logNotice("Pushing '%s' to queue..", app->service().toAscii().data());
    m_queue.push(app);
}

void Prestarter::sendPrestartRequest(PrestartApp * app)
{
    // Check that the app is not already prestarted
    if (!app->prestarted())
    {
        app->setPrestarted(false);
        app->setReleased(false);
        app->setPid(0);

        // Create and send a launch message. This will start the application
        // service
        QDBusMessage message(QDBusMessage::createMethodCall(app->service(),
            "/", "", "ping"));
        m_connection.call(message, QDBus::NoBlock);
    }
    else
    {
        Logger::logWarning("'%s' already prestarted!", app->service().toAscii().data());
    }
}

void Prestarter::killPrestartedApplications()
{
    // Try to find prestared application with the given PID
    Q_FOREACH(PrestartApp * p, m_vectApps)
    {
        p->setRePrestarting(false);
        killApp(p);
    }
}

Prestarter::~Prestarter()
{
    // Delete the apps since they are not needed anymore
    Q_FOREACH(PrestartApp * p, m_vectApps)
    {
        delete p;
    }
}
