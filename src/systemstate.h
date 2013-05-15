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

#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include <QObject>
#include <tr1/memory>
#include "contextproperty.h"

using std::tr1::shared_ptr;

class QTextStream;
class QSocketNotifier;
class QTimer;
class CPULoad;

/*! \class SystemState 
 *  \brief SystemState provides information about the system to be used by the Daemon.
 *
 *  SystemState class is used to fetch some data about the system that can affect
 *  e.g. how prestarting is done. One important value is the current CPU-load.
 *
 *  SystemState can also be run in test mode, which means that it listens to commands
 *  from stdin. This feature can be used in functional tests for Applifed.
 *
 *  Accepted commands in the test mode at the moment are:
 *
 *  EXIT               : Exit the program immediately.
 *  EXIT_WITH_DELAY=%d : Exit the program after delay of %d seconds.
 *  PRESTART=%s        : Execute prestarts from the given config %s.
 *  CPU_LOAD=%d        : Force (fake) CPU-load to the given value.
 *  SCREEN_BLANKED     : Force (fake) screen blank. This triggers re-prestarting routines.
 *  SCREEN_UNBLANKED   : Force (fake) screen unblank. This cancels re-prestarting routines.
 *  SCREEN_BLANKED_WITH_DELAY=%d     : Force (fake) screen blank with delay.
 *                       This triggers re-prestarting routines after a delay of %d seconds.
 */
class SystemState : public QObject
{
    Q_OBJECT

public:

    /*! Constructor
     * \param testMode If set to true then system data is fetched from test input
     */
    explicit SystemState(bool testMode = false);

    /*! \brief Enable / disable CPU load polling.
     *  \param enable Enables polling if true, disables it if false.
     *  \param interval Polling interval in seconds.
     */
    void enableCPULoadPolling(bool enable, int interval = 1);

Q_SIGNALS:

    //! System memory is low
    void systemMemoryLow();

    //! Request prestarting (for testing purposes)
    void prestartRequested(const QString & prestartPath);

    //! Signal that we have new CPU load data
    void cpuLoadUpdated(int);

    //! Signal that screen has blanked / unblanked
    void screenBlanked(bool);

private Q_SLOTS:

    //! Read test commands from stdin
    void readTestData(int);

    //! Emit cpuLoadUpdated-signal
    void emitCPULoadUpdated();

    //! Exit
    void exitSlot();

    //! Screen_blank with delay
    void screenBlankWithDelaySlot();

    //! ContextProperty("Session.State") value changed (Screen blanked / unblanked)
    void screenStateChanged();

private:

    Q_DISABLE_COPY(SystemState)

    //! Parse a test data line
    void parseLine(const QString & line);

    //! Use user sources instead of real system is set to true
    bool m_testMode;

    //! Stream from stdin (test mode)
    shared_ptr<QTextStream> m_stdin;

    //! Notifier to signal when new data had arrived to stdin (test mode)
    shared_ptr<QSocketNotifier> m_notifier;

    //! Timer used in CPU load polling
    shared_ptr<QTimer> m_pollTimer;

    //! Timer used in delayed exit in test mode
    shared_ptr<QTimer> m_exitTimer;

    //! Timer used in delayed screen_blanked in test mode
    shared_ptr<QTimer> m_screenBlankWithDelayTimer;

    //! CPU load measurer
    shared_ptr<CPULoad> m_cpuLoad;

    //! True, if CPU load polling is enabled. Internal state variable.
    bool m_cpuLoadPollingEnabled;

    //! True, if exitting with a delay is active
    bool m_exitWithDelay;

    //! True, if screen_blank with a delay is active
    bool m_screenBlankWithDelay;

    //! Information about screen blanking from Context Framework
    ContextProperty *m_screenState;

    //! Information about battery state from Context Framework
    ContextProperty *m_batteryState;
};

#endif // SYSTEMSTATE_H
