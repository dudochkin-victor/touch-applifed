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

#ifndef DAEMON_H
#define DAEMON_H

#include <QApplication>
#include <QString>
#include <QStringList>

#include <tr1/memory>

using std::tr1::shared_ptr;

#include <vector>

using std::vector;

#include "logger.h"

#include <X11/Xlib.h>

class SystemState;
class Prestarter;
class QSocketNotifier;

/*!
 * \class Daemon main class
 * \brief Daemon provides daemonizing and prestarting functionality
 *
 */
class Daemon : public QApplication
{
    Q_OBJECT

public:

    /*!
     * \brief Construct Daemon object in particular mode.
     */
    Daemon(int & argc, char * argv[], bool testMode, QString logFile);

    /*!
     * \brief Destroy Daemon object.
     */
    virtual ~Daemon();

    /*!
     * \brief  Start the daemon.
     * \return Exit code
     */
    int run();

    //! Return the prestarter object
    Prestarter * prestarter() const;

    //! Send SIG_TERM to X11 programs
    void terminate();

    //! Return current Daemon instance
    static Daemon * instance();

    /*! \brief  Acquire lock file to prevent launch of second instance.
     *  \return True if succeeds.
     */
    static bool lock();

    //! Check that lock is free. Do nothing if ok,
    //! otherwise print error to stderr and ::exit().
    static void checkForLock(bool testMode);

    //! Fork to a daemon
    static void daemonize(bool testMode);

    //! UNIX signal handler for SIGTERM
    static void termSignalHandler(int unused);

private:

    //! Disable copy-constructor
    Daemon(const Daemon & r);

    //! Disable assignment operator
    Daemon & operator= (const Daemon & r); 

    //! \reimp
    virtual bool x11EventFilter(XEvent *event);

    //! Handler for XMapEvent
    void handleXMapEvent(XMapEvent * event);

    //! Handler for XUnmapEvent
    void handleXUnmapEvent(XUnmapEvent * event);

    //! Singleton daemon instance
    static Daemon * m_instance;

    //! Test mode flag
    bool m_testMode;

    //! Prestarter object
    shared_ptr<Prestarter> m_prestarter;

    //! System state object
    shared_ptr<SystemState> m_systemState;

    //! Log file if syslog not used. Override with --log.
    QString m_logFile;

    //! File descriptor for the lock file
    static int m_lockFd;

    //! Socket pair used to get SIGTERM
    static int m_sigtermFd[2];

    //! Socket notifier used for m_sigtermFd
    shared_ptr<QSocketNotifier> m_snTerm;

private slots:

    //! Qt signal handler for SIGTERM.
    void handleSigTerm();

    /*! Prestart applications
     * \param prestartPath Path to search .prestart files in
     */
    void runPrestarts(const QString & prestartPath);
};

#endif // DAEMON_H

