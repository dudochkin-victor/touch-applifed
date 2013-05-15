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

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include <QSocketNotifier>

#include "prestarter.h"
#include "systemstate.h"
#include "daemon.h"
#include "x11util.h"

Daemon * Daemon::m_instance = NULL;
int      Daemon::m_lockFd = -1;
int      Daemon::m_sigtermFd[2];

Daemon::Daemon(int & argc, char * argv[], bool testMode, QString logFile) :
    QApplication(argc, argv),
    m_testMode(testMode),
    m_prestarter(),
    m_systemState(),
    m_logFile(DEFAULT_LOG_FILE)
{
    if (!logFile.isEmpty())
    {
        m_logFile = logFile;
    }

    Daemon::m_instance = this;

    // Open the log
    Logger::openLog(PROG_NAME, m_logFile.toStdString().c_str());
    Logger::logNotice("%s starting..", PROG_NAME);

    // Create components
    m_prestarter.reset(new Prestarter(m_testMode));
    m_systemState.reset(new SystemState(m_testMode));

    // Check whether we are in test mode
    if (m_testMode)
    {
        // Connect test signal for prestart
        connect(m_systemState.get(), SIGNAL(prestartRequested(const QString &)),
                this, SLOT(runPrestarts(const QString &)));
    }

    connect(m_systemState.get(), SIGNAL(screenBlanked(bool)),
            m_prestarter.get(), SLOT(handleScreenBlanked(bool)));

    // Init X11 snooping for the root window
    Display * dpy = QX11Info::display();
    if (dpy)
    {
        XSelectInput(dpy, RootWindow(dpy, XDefaultScreen(dpy)),
                     SubstructureNotifyMask | VisibilityChangeMask);
    }
    else
    {
        Logger::logWarning("Cannot open display!");
    }

    // Create socket pair for SIGTERM
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_sigtermFd))
    {
       Logger::logError("Couldn't create TERM socketpair");
       ::exit(EXIT_FAILURE);
    }

    // Install a socket notifier on the socket
    m_snTerm.reset(new QSocketNotifier(m_sigtermFd[1], QSocketNotifier::Read, this));
    connect(m_snTerm.get(), SIGNAL(activated(int)), this, SLOT(handleSigTerm()));
}

//
// All this signal handling code is taken from Qt's Best Practices:
// http://doc.qt.nokia.com/latest/unix-signals.html
//

void Daemon::termSignalHandler(int)
{
    char a = 1;
    ::write(m_sigtermFd[0], &a, sizeof(a));
}

void Daemon::handleSigTerm()
{
    m_snTerm->setEnabled(false);

    char tmp;
    ::read(m_sigtermFd[1], &tmp, sizeof(tmp));

    Daemon::terminate();

    m_snTerm->setEnabled(true);
}

void Daemon::terminate()
{
    m_prestarter->killPrestartedApplications();

    QApplication::quit();
}

Daemon * Daemon::instance()
{
    return Daemon::m_instance;
}

bool Daemon::x11EventFilter(XEvent * xEvent)
{
    if (xEvent->type == MapNotify)
    {
        // We handle MapNotify's to track applications that have been
        // released from the prestarted state.

        handleXMapEvent(reinterpret_cast<XMapEvent*>(xEvent));

        // Event handled
        return true;
    }
    else if (xEvent->type == UnmapNotify)
    {
        // We handle UnmapNotify's to track applications that have been
        // returned to the prestarted state (lazy shutdown). If all windows
        // of a service have been closed, the service is considered to
        // restore the prestarted state.

        handleXUnmapEvent(reinterpret_cast<XUnmapEvent*>(xEvent));

        // Event handled
        return true;
    }

    // Event not handled
    return false;
}

void Daemon::handleXMapEvent(XMapEvent * event)
{
    const int pid = X11Util::windowPid(event->window);
    if (pid != -1)
    {
        // Notify Prestarter about application possibly being released
        // from the prestarted state. Prestarter verifies the pid.
        m_prestarter->releasedPrestart(pid);
    }
}

void Daemon::handleXUnmapEvent(XUnmapEvent * event)
{
    // Find out pid assigned to the window
    const int pid = X11Util::windowPid(event->window);
    if (pid != -1)
    {
        // Check already here that this pid is assigned to some
        // prestarted application
        if (m_prestarter->findAppWithPid(pid))
        {
            // Hidden windows are not included in the client list.
            // We can then just check that there are no windows for
            // this pid mapped.
            if (X11Util::windowsForPid(pid).isEmpty())
            {
                // Notify Prestarter about application being restored to
                // the prestarted state. Prestarter verifies the pid.
                m_prestarter->restoredPrestart(pid);
            }
        }
        else
        {
            // Here we handle applications that are obviously not prestarted by
            // applifed. The MeeGo Touch FW sets _MEEGOTOUCH_PRESTARTED flag to
            // windows that have gone to the prestarted state. We check this and
            // get the application binary path responsible for the UnmapNotify.

            if (X11Util::isPrestarted(event->window))
            {
                const QString appName = X11Util::applicationName(event->window);
                if (!m_prestarter->isLazyShutdownAllowed(appName))
                {
                    const int pid = X11Util::windowPid(event->window);
                    Logger::logNotice("External app '%s' (%d) performed an unauthorized lazy shutdown: will be killed",
                                      appName.toAscii().data(), pid);
                    if (pid != -1)
                    {
                        if (kill(pid, SIGKILL) != -1)
                        {
                            Logger::logNotice("'%s' (%d) killed", appName.toAscii().data(), pid);
                        }
                        else
                        {
                            if (errno == EPERM)
                            {
                                Logger::logError("No permissions to kill %d", pid);
                            }
                            else if (errno == ESRCH)
                            {
                                Logger::logError("Pid %d doesn't exist or may be a zombie", pid);
                            }
                        }
                    }
                }
                else
                {
                    Logger::logNotice("External application '%s' returned to prestarted state",
                                      appName.toAscii().data());
                }
            }
        }
    }
}

void Daemon::daemonize(bool testMode)
{
    // Our process ID and Session ID 
    pid_t pid, sid;

    // Fork off the parent process: first fork
    pid = fork();
    if (pid < 0)
    {
        std::cerr << "Unable to fork daemon: " << strerror(errno) << std::endl;
        ::exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process.
    if (pid > 0)
    {
        ::exit(EXIT_SUCCESS);
    }

    // Fork off the parent process: second fork
    pid = fork();
    if (pid < 0)
    {
        std::cerr << "Unable to fork daemon: " << strerror(errno) << std::endl;
        ::exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process.
    if (pid > 0)
    {
        ::exit(EXIT_SUCCESS);
    }

    // Check that the lock is free
    checkForLock(testMode);

    // Change the file mode mask 
    umask(0);

    // Open any logs here 

    // Create a new SID for the child process 
    sid = setsid();
    if (sid < 0)
    {
        std::cerr << "Unable to create a new session: " << strerror(errno) << std::endl;
        ::exit(EXIT_FAILURE);
    }

    // Change the current working directory 
    if ((chdir("/")) < 0)
    {
        std::cerr << "Unable to change directory to '/': " << strerror(errno) << std::endl;
        ::exit(EXIT_FAILURE);
    }

    // Open file descriptors pointing to /dev/null
    const int new_stdin  = open("/dev/null", O_RDONLY);
    const int new_stdout = open("/dev/null", O_WRONLY);
    const int new_stderr = open("/dev/null", O_WRONLY);

    // Redirect standard file descriptors to /dev/null
    dup2(new_stdin,  STDIN_FILENO);
    dup2(new_stdout, STDOUT_FILENO);
    dup2(new_stderr, STDERR_FILENO);

    // Close new file descriptors
    close(new_stdin);
    close(new_stdout);
    close(new_stderr);
}

int Daemon::run()
{
    // Prestart applications if not in test mode
    if (!m_testMode)
    {
        runPrestarts(CONFIG_FILE);
    }

    // Start Qt mainloop
    return exec();
}

void Daemon::runPrestarts(const QString & prestartPath)
{
    // Echo log messages if in test mode
    Logger::setEchoMode(m_testMode);

    Logger::logNotice("Running initial prestarts from '%s'..",
                      prestartPath.toStdString().c_str());

    if (m_prestarter->readConfiguration(prestartPath))
    {
        m_prestarter->startPrestartActions();
    }
    else
    {
        ::exit(EXIT_FAILURE);
    }
}

Prestarter * Daemon::prestarter() const
{
    return m_prestarter.get();
}

bool Daemon::lock()
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 1;

    if((m_lockFd = open(LOCK_FILE, O_WRONLY | O_CREAT, 0666)) == -1)
        return false;

    if(fcntl(m_lockFd, F_SETLK, &fl) == -1)
        return false;

    return true;
}

void Daemon::checkForLock(bool testMode)
{
    if(!testMode && !Daemon::lock())
    {
        std::cerr << "Only one instance of " << PROG_NAME <<
                " can be running" << std::endl;

        ::exit(EXIT_FAILURE);
    }
}

Daemon::~Daemon()
{
    // Close the log
    Logger::closeLog();
}

