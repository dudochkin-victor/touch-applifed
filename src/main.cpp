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

#include <syslog.h>
#include <cstdlib>
#include <signal.h>

#include <QCoreApplication>
#include <QString>
#include <vector>
#include <iostream>

#include "daemon.h"
#include "logger.h"

static void usage()
{
    std::cout << "Usage: " << PROG_NAME << " [options]\n"
    << "\n"
    << "Options:\n"
    << "  --fork              Run as a daemon.\n"
    << "  --log [LOG_FILE]    Specify the log file.\n"
    << "  --test              Run in test mode.\n"
    << "  --version           Print program version.\n"
    << "  --help              Print this help message.\n"
    << std::endl;
}

//! Parse arguments
typedef vector<QString> ArgVect;
static void parseArgs(const ArgVect & args, bool * daemonize, bool * testMode, QString * logFile)
{
    for (ArgVect::const_iterator i(args.begin());
        i != args.end(); i++)
    {
        if ((*i) == "--help")
        {
            usage();
            exit(EXIT_SUCCESS);
        }
        else if ((*i) == "--fork")
        {
            *daemonize = true;
        }
        else if ((*i) == "--test")
        {
            *testMode = true;
        }
        else if ((*i) == "--log")
        {
            if (i + 1 != args.end())
            {
                *logFile = (*(i + 1));
                i++;
            }
        }
    }
}

static bool setupUnixSignalHandlers()
{
    struct sigaction term;

    term.sa_handler = Daemon::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
    {
        return false;
    }

    return true;
}

//! Main function
int main(int argc, char * argv[])
{
    bool    daemonize = false;
    bool    testMode  = false;
    QString logFile   = "";

    // Parse arguments
    parseArgs(ArgVect(argv, argv + argc), &daemonize, &testMode, &logFile);

    // Install Unix signal handler for SIGTERM
    if (!setupUnixSignalHandlers())
    {
        std::cerr << "Failed to install Unix signal handlers" << std::endl;
        ::exit(EXIT_FAILURE);
    }

    // Check that an instance of applifed is not already running if we are
    // not in test mode
    Daemon::checkForLock(testMode);

    // Daemonize (Note: this must take place before Daemon is created, because
    // it's a QApplication)
    if (daemonize)
    {
        Daemon::daemonize(testMode);
    }

    // Main daemon object
    Daemon myDaemon(argc, argv, testMode, logFile);

    // Run the main loop
    return myDaemon.run();
}
