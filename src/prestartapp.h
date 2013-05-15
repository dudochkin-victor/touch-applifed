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

#ifndef PRESTARTAPP_H
#define PRESTARTAPP_H

#include <QString>

/*!
 * \class PrestartApp
 * \brief Config struct for a prestartable application.
 *
 */
class PrestartApp
{
public:

    /*!
     * \brief Construct a PrestartApp object with \a path and list of \a params.
     * \param name     Name of the application
     * \param service  D-Bus service name of the application binary
     * \param priority Prestart priority.
     *
     * Applications with priority level less than zero will be prestarted
     * immediately, one by one, when applifed has been started and before it
     * daemonizes itself. Applications with the smallest priority level will be
     * started first.
     *
     * Applications with priority level greater or equal to zero will be
     * prestarted when system load goes down and there is still enough memory.
     * In practice, this means that desktop is already up and running.
     */
    PrestartApp(QString name, QString service, int priority = 0, bool testMode = false);

    /*! 
     * \brief Add \a param to application's parameters list
     * \param param New parameter for application
     */
    void addParam(const QString & param);

    /*! 
     * \brief Get service name
     * \return D-Bus service name
     */
    const QString & service() const;

    /*!
     * \brief Get application name
     * \return Application name
     */
    const QString & name() const;

    //! Return true if the app is currently released from prestarted state
    bool released() const;

    //! Set the released state
    void setReleased(bool released);

    /*! \brief Return how many times app has been released.
     * This is automatically updated by calling setReleased() so that
     * current released-state changes from false => true.
     */
    unsigned int releasedCount() const;

    //! Set releasedCount to zero.
    void resetReleasedCount();

    /*! If releasedCount() exceeds this value, the service should
     *  be scheduled for re-prestart.
     */
    unsigned int rePrestartTrigger() const;

    //! Set rePrestartTrigger to the given value.
    void setRePrestartTrigger(unsigned int value);

    //! Returns true if the app is currently scheduled fo re-prestart.
    bool rePrestarting() const;

    /*! Set rePrestarting state to the given value.
     *  Clear m_sigTermSent if state == false.
     */
    void setRePrestarting(bool state);

    //! Return true if forced re-prestart for this app is not allowed
    bool forcedRePrestartNotAllowed() const;

    //! Allow/deny forced re-prestart
    void setForcedRePrestartNotAllowed(bool state);

    //! Return true if app was tried to term with SIGTERM
    bool sigTermSent() const;

    //! Set if app was tried to term with SIGTERM
    void setSigTermSent(bool state);

    //! Return true if app was tried to term with SIGKILL
    bool sigKillSent() const;

    //! Set if app was tried to term with SIGKILL
    void setSigKillSent(bool state);

    //! Return true if the app is prestarted (released or not)
    bool prestarted() const;

    /*! Set the prestarted state. Must be set to false, if the
     *  app terminates.
     */
    void setPrestarted(bool prestarted);

    //! Set the prestart priority
    void setPriority(int newPriority);

    //! Set pid of the application
    void setPid(unsigned int newPid);

    //! Set the CPU-load threshold
    void setCPULoadThreshold(int value);

    //! Get the prestart priority
    int priority() const;

    /*! Get the CPU-load threshold
     * \return -1 if not set.
     */
    int cpuLoadThreshold() const;

    //! Get the pid
    unsigned int pid() const;

    //! True if a test app
    bool testMode() const;

private:

    //! Service name of the app
    QString m_service;

    //! Application name
    QString m_name;

    //! True, if the app is released from prestarted state
    bool m_released;

    //! Number of times app has been released
    unsigned int m_releasedCount;

    //! True, if the app is prestarted (released or not)
    bool m_prestarted;

    //! True, if the app is scheduled for re-prestart
    bool m_rePrestarting;

    //! True, if forced re-prestart is not allowed for this app
    bool m_forcedRePrestartNotAllowed;

    //! Prestart priority
    int m_priority;

    //! CPU load threshold to be used with this app
    int m_cpuLoadThreshold;

    //! pid
    unsigned int m_pid;

    /*! If releasedCount() exceeds this value, the service should
     *  be scheduled for re-prestart.
     */
    unsigned int m_rePrestartTrigger;

    //! True if app was tried to term with SIGTERM
    bool m_sigTermSent;

    //! True if app was tried to term with SIGKILL
    bool m_sigKillSent;

    //! True if a test app only
    bool m_testMode;
};

#endif // PRESTARTAPP_H
