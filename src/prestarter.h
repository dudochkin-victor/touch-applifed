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

#ifndef PRESTARTER_H
#define PRESTARTER_H

#include <QString>
#include <QStringList>
#include <QSet>
#include <QVector>
#include <QStack>
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QTimer>
#include <QXmlInputSource>

#include <queue>

using std::priority_queue;

#include <tr1/memory>

using std::tr1::shared_ptr;

#include "prestartapp.h"

class SystemState;

/*!
 * \class Prestarter
 * \brief Prestarter class that prestarts applications.
 *
 *    Description about how it goes:
 *
 * 1) Prestarter reads the information about which D-Bus services should be prestarted
 *    (/etc/prestart/). It is assumed, that the corresponding applications gets started
 *    with -prestart and are MeeGo Touch applications. -prestart should be set in their
 *    .service files. Prestart activities are started by calling startPrestartActions()
 *    after readConfiguration().
 *
 * 2) Prestarter pushes applications to be prestarted to a priority queue (sorted with
 *    respect to their priority-values defined in the config. Smaller priority value
 *    means greater importance.
 *
 * 3) CPU load measurement is started. /proc/stat is used and the value is periodically
 *    updated. On each update Prestarter checks if the load is below given threshold,
 *    and if so, it prestarts the next application in the queue. If the queue gets empty
 *    then the CPU load polling is stopped and Prestarter does nothing.
 *
 * 4) Prestarter monitors D-Bus (using QDBusServiceWatcher)and if it notices that a
 *    prestarted service ended (died or was closed by the user) it gets pushed back to the
 *    queue and CPU load measurement is started again (re-prestarting). Prestarter snoops
 *    X11 events so that it knows if a prestarted service shows its UI. A service cannot
 *    be re-prestarted without showing its UI. This effectively prevents a prestart-loop
 *    of a crashing application.
 *
 * 5) Applifed knows how many times each prestarted application has returned to the prestarted
 *    state and may re-prestart them if the count exceeds the given limit. Applications are
 *    re-prestarted only when screen goes blank and some time has elapsed. This behaviour
 *    can be configured in the configuration file.
 *
 *    Note that there are two kinds of re-prestarting:
 *
 *    - Re-prestarting due to a crash (this happens immediately)
 *    - Re-prestarting due to release count (this happens after screen has blanked)
 *
 * 6) Applications performing unauthorized (not prestarted by applifed nor white-listed
 *    in the configuration file) lazy shutdown are killed. This is triggered by the Daemon.
 */
class Prestarter : public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief Constructor.
     */
    explicit Prestarter(bool testMode = false);

    /*!
     * \brief Destructor.
     */
    virtual ~Prestarter();

    /*! 
     * \brief Read the prestarting configuration.
     * \return <code>true</code> if the configuration was successfully read, <code>false</code> otherwise */
    bool readConfiguration(const QString & configPath);

    /*! 
     * \brief Start prestarting applications defined in the configuration
     */
    void startPrestartActions();

    /*!
     * \brief Return the application having the given service.
     * \param service Service to be matched
     * \return NULL if not found
     */
    PrestartApp * findAppWithService(const QString & service) const;

    /*!
     * \brief Return the application having the given PID.
     * \param pid Pid to be matched
     * \return NULL if not found
     */
    PrestartApp * findAppWithPid(unsigned int pid) const;

    /*! Return true if lazy shutdown for the given executable is allowed.
     *  Note: This is for apps not prestarted by applifed. Can be configured
     *  with "allow_lazy_shutdown" in the config file.
     */
    bool isLazyShutdownAllowed(const QString & path) const;

public Q_SLOTS:

    /*!
     * \brief Notify Prestarter about a process having returned
     *        to the pre-started state.
     * \param pid PID of the process. Affects all apps if pid == 0.
     */
    void restoredPrestart(unsigned int pid);

    /*!
     * \brief Notify Prestarter about a process having released
     *        from the pre-started state.
     * \param pid PID of the process. Affects all apps if pid == 0.
     */
    void releasedPrestart(unsigned int pid);

    //! Handle blanked / unblanked screen. Starts forced re-prestarts.
    void handleScreenBlanked(bool state);

    //! Permanently terminate prestarted applications (both released and not-released).
    void killPrestartedApplications();

private Q_SLOTS:

    //! Start re-prestarting apps that are scheduled for forced re-prestart
    void triggerForcedRePrestarts();

    //! Called when a new D-Bus service gets registered
    void handleRegisteredService(const QString & service);

    //! Called when a new D-Bus service gets unregistered
    void handleUnregisteredService(const QString & service);

    //! Called when a new D-Bus service owner gets changed
    void handleServiceOwnerChange(const QString & name, const QString & oldOwner, const QString & newOwner);

    /*! Called when a new CPULoad value is ready. If CPU load is low enough
     *  this causes the next application to be taken from the queue and
     *  getting prestarted.
     */
    void updateCPULoad(int newLoad);

private:

    //! Internal states
    enum State {Init, WaitForPrestart, WaitForCPU};

    //! Compare class for the priority queue
    struct ComparePriority
    {
        bool operator() (PrestartApp * pLhs, PrestartApp * pRhs)
        {
            return pLhs->priority() > pRhs->priority();
        }
    };

    /*! 
     * \brief Disable copy-constructor
     */
    Prestarter(const Prestarter & r);

    /*!
     * \brief Disable assignment operator
     */
    Prestarter & operator= (const Prestarter & r); 

    //! Read configuration from the given source
    bool doReadConfiguration(const QXmlInputSource & source);

    /*! Starts CPU load polling and connects update signals.
     *  Polling is automatically stopped when nothing to prestart.
     */
    void startCPULoadPolling();

    //! Stops CPU load polling and disconnects update signals
    void stopCPULoadPolling();

    //! Return true if at least one app is scheduled for forced re-prestart
    bool somethingToRePrestart() const;

    /*!
     * \brief Prestart and launch the given app
     * \param path Path to the binary to be prestarted
     * \param service Related D-Bus service. Prestater waits for this to be registered
     *        on the session bus before trying to launch the app.
     */
    void prestartAndLaunch(const QString & path, const QString & service);

    /*!
     * \brief Add an app to be prestarted.
     * \param p Pointer to the PrestartApp structure. Prestarter
     *        will take the ownership of p. NULL's are discarded.
     */
    void addPrestartApp(PrestartApp * p);

    /*! 
     * \brief Prestart an application via D-Bus.
     * \param app Application to be prestarted
     */
    void sendPrestartRequest(PrestartApp * app);

    /*! 
     * \brief Place application to the prestart queue.
     * \param app Application to be prestarted
     * \return true if success
     */
    void pushApplicationToPrestartQueue(PrestartApp * app);

    //! Run the current state
    void runState();

    //! Set internal state
    void setState(State newState);

    //! Init state
    void stateInit();

    //! State for checking if something to prestart
    void stateWaitForPrestart();

    //! State for checking if CPU load is low enough
    void stateWaitForCPU();

    //! Kill application. First try SIGTERM, then SIGKILL.
    void killApp(PrestartApp * p);

    //! True, if running in the test mode
    bool m_testMode;

    /*! 
     * \brief Path to find config files of prestartable apps.
     */
    QString m_configPath;

    /*! 
     * \brief Vector of prestartable apps in the config.
     */
    QVector<PrestartApp *> m_vectApps;

    /*!
     * \brief Priority queue of prestartable apps
     */
    priority_queue<PrestartApp *, QVector<PrestartApp *>, ComparePriority> m_queue;

    //! \brief Session bus connection
    QDBusConnection m_connection;

    //! \brief Service watcher to notify about registered and unregistered services
    QDBusServiceWatcher m_serviceWatcher;

    //! System state information
    shared_ptr<SystemState> m_systemState;

    //! True if CPU load polling is currently active / enabled
    bool m_cpuLoadPollingEnabled;

    //! Internal state
    State m_state;

    //! Current CPU load
    int m_load;

    //! CPU polling delay in seconds
    int m_cpuLoadPollingDelay;

    //! Delay before forced re-prestarts after screen has blanked
    int m_rePrestartDelay;

    //! Timer for forced re-prestarts
    QTimer m_forcedRePrestartTimer;

    //! List of lazily shutdownable applications not prestarted by applifed
    QStringList m_allowedLazyShutdownApps;

#ifdef UNIT_TEST
    friend class Ut_Prestarter;
#endif
};

#endif // PRESTARTER_H
