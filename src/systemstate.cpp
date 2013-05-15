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

#include "systemstate.h"
#include "cpuload.h"

#include <QTextStream>
#include <QSocketNotifier>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>

#include <cstdio>
#include <iostream>

SystemState::SystemState(bool testMode) :
    m_testMode(testMode),
    m_pollTimer(new QTimer),
    m_exitTimer(new QTimer),
    m_screenBlankWithDelayTimer(new QTimer),
    m_cpuLoad(new CPULoad),
    m_cpuLoadPollingEnabled(false),
    m_exitWithDelay(false),
    m_screenBlankWithDelay(false),
    m_screenState(new ContextProperty("Session.State")),
    m_batteryState(new ContextProperty("Battery.ChargePercentage"))
{
    connect(m_screenState, SIGNAL(valueChanged()), this, SLOT(screenStateChanged()));

    if (m_testMode)
    {
        // Connect stdin
        m_stdin.reset(new QTextStream(stdin, QIODevice::ReadOnly));
        m_notifier.reset(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read));
        connect(m_notifier.get(), SIGNAL(activated(int)), this, SLOT(readTestData(int)));

        // Connect exit timer
        connect(m_exitTimer.get(), SIGNAL(timeout()), this, SLOT(exitSlot()));
        
        // Connect Sceen_Blanked_with_delay timer
        connect(m_screenBlankWithDelayTimer.get(), SIGNAL(timeout()), this, SLOT(screenBlankWithDelaySlot()));
    }
}

void SystemState::readTestData(int)
{
    QString line = m_stdin->readLine();
    parseLine(line);

    // Check if we are at the end of the stream
    if (m_stdin->atEnd())
    {
        // Exit now if EXIT_WITH_DELAY wasn't set
        if (!m_exitWithDelay)
        {
            parseLine(QString("EXIT"));
        }

        // Stop notifier, because it seems to busy loop after
        // stream from stdin ends.
        m_notifier->setEnabled(false);
    }
}

void SystemState::parseLine(const QString & line)
{
    // Exit the daemon
    if (line == "EXIT")
    {
        QCoreApplication::exit(0);
    }
    // Exit the daemon with a delay
    else if (line.startsWith("EXIT_WITH_DELAY="))
    {
        QStringList dummy(line.split('='));
        if (dummy.length() == 2)
        {
            m_exitWithDelay = true;
            m_exitTimer->setInterval(dummy.at(1).toInt() * 1000);
            m_exitTimer->setSingleShot(true);
            m_exitTimer->start();
        }
    }
    // screen_blanked with a delay
    else if (line.startsWith("SCREEN_BLANKED_WITH_DELAY="))
    {
        QStringList dummy(line.split('='));
        if (dummy.length() == 2)
        {
            m_screenBlankWithDelay = true;
            m_screenBlankWithDelayTimer->setInterval(dummy.at(1).toInt() * 1000);
            m_screenBlankWithDelayTimer->setSingleShot(true);
            m_screenBlankWithDelayTimer->start();
        }
    }
    // Send the memory low
    else if (line == "MEMORY_LOW")
    {
        emit systemMemoryLow();
    }
    // Set CPU-load to some value
    // e.g. CPU_LOAD=50
    else if (line.startsWith("CPU_LOAD="))
    {
        QStringList dummy(line.split('='));
        if (dummy.length() == 2)
        {
            // Force CPU-load detector to give this value from
            // now on
            m_cpuLoad->forceValue(dummy.at(1).toInt());
        }
    }
    // Run prestarts from the given location
    // e.g. PRESTART=/tmp
    else if (line.startsWith("PRESTART="))
    {
        QStringList dummy(line.split('='));
        if (dummy.length() == 2)
        {
            emit prestartRequested(dummy.at(1));
        }
    }
    // Fake blanked screen
    else if (line.startsWith("SCREEN_BLANKED"))
    {
        emit screenBlanked(true);
    }
    // Fake unblanked screen
    else if (line.startsWith("SCREEN_UNBLANKED"))
    {
        emit screenBlanked(false);
    }
    else
    {
        if (!line.isEmpty())
        {
            std::cerr << "ERROR!!: Unknown command '" << line.toStdString() << "'" << std::endl;
        }
    }
}

void SystemState::exitSlot()
{
    QCoreApplication::exit(0);
}

void SystemState::screenBlankWithDelaySlot()
{
    emit screenBlanked(true);
    std::cerr << "Screen Blanked With Delay" << std::endl;
}

void SystemState::screenStateChanged()
{
    if (!m_testMode)
    {
        if (m_screenState->value().toString() == "blanked") 
        {
            emit screenBlanked(true);
        }
        else 
        {
            emit screenBlanked(false);
        }
    }
}

void SystemState::enableCPULoadPolling(bool enable, int interval)
{
    if (enable)
    {
        if (!m_cpuLoadPollingEnabled)
        {
            connect(m_pollTimer.get(), SIGNAL(timeout()), this, SLOT(emitCPULoadUpdated()));

            m_cpuLoad->init();
            m_pollTimer->setInterval(interval * 1000);
            m_pollTimer->start();

            m_cpuLoadPollingEnabled = true;
        }
    }
    else
    {
        disconnect(m_pollTimer.get(), SIGNAL(timeout()), this, SLOT(emitCPULoadUpdated()));

        m_pollTimer->stop();

        m_cpuLoadPollingEnabled = false;
    }
}

void SystemState::emitCPULoadUpdated()
{
    m_cpuLoad->update();
    if (m_cpuLoad->getValue() != -1)
    {
        emit cpuLoadUpdated(m_cpuLoad->getValue());
    }
}
