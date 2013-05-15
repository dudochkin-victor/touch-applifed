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

#include "prestartapp.h"

PrestartApp::PrestartApp(QString name, QString service, int priority, bool test) :
    m_service(service),
    m_name(name),
    m_released(false),
    m_releasedCount(0),
    m_prestarted(false),
    m_rePrestarting(false),
    m_forcedRePrestartNotAllowed(false),
    m_priority(priority),
    m_cpuLoadThreshold(0),
    m_pid(0),
    m_rePrestartTrigger(0),
    m_sigTermSent(false),
    m_sigKillSent(false),
    m_testMode(test)
{}

const QString & PrestartApp::service() const
{
    return m_service;
}

const QString & PrestartApp::name() const
{
    return m_name;
}

bool PrestartApp::released() const
{
    return m_released;
}

void PrestartApp::setReleased(bool newReleased)
{
    // Increase release counter
    if (newReleased)
    {
        if (!m_released)
        {
            m_releasedCount++;
        }
    }

    m_released = newReleased;
}

unsigned int PrestartApp::releasedCount() const
{
    return m_releasedCount;
}

void PrestartApp::resetReleasedCount()
{
    m_releasedCount = 0;
}

unsigned int PrestartApp::rePrestartTrigger() const
{
    return m_rePrestartTrigger;
}

void PrestartApp::setRePrestartTrigger(unsigned int value)
{
    m_rePrestartTrigger = value;
}

bool PrestartApp::rePrestarting() const
{
    return m_rePrestarting;
}

void PrestartApp::setRePrestarting(bool state)
{
    m_rePrestarting = state;

    if (!state)
    {
        setSigTermSent(false);
        setSigKillSent(false);
    }
}

bool PrestartApp::forcedRePrestartNotAllowed() const
{
    return m_forcedRePrestartNotAllowed;
}

void PrestartApp::setForcedRePrestartNotAllowed(bool value)
{
    m_forcedRePrestartNotAllowed = value;
}

bool PrestartApp::sigTermSent() const
{
    return m_sigTermSent;
}

void PrestartApp::setSigTermSent(bool state)
{
    m_sigTermSent = state;
}

bool PrestartApp::sigKillSent() const
{
    return m_sigKillSent;
}

void PrestartApp::setSigKillSent(bool state)
{
    m_sigKillSent = state;
}

bool PrestartApp::prestarted() const
{
    return m_prestarted;
}

void PrestartApp::setPrestarted(bool newPrestarted)
{
    if (!newPrestarted)
    {
        setRePrestarting(false);
        resetReleasedCount();
        setReleased(false);
    }

    m_prestarted = newPrestarted;
}

int PrestartApp::priority() const
{
    return m_priority;
}

void PrestartApp::setPriority(int newPriority)
{
    m_priority = newPriority;
}

void PrestartApp::setPid(unsigned int newPid)
{
    m_pid = newPid;
}

unsigned int PrestartApp::pid() const
{
    return m_pid;
}

void PrestartApp::setCPULoadThreshold(int value)
{
    m_cpuLoadThreshold = value;
}

int PrestartApp::cpuLoadThreshold() const
{
    return m_cpuLoadThreshold;
}

bool PrestartApp::testMode() const
{
    return m_testMode;
}
