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

#include "configdata.h"

ConfigData::ConfigData() :
    m_prestartApps(),
    m_allowedApps(),

    // These can be overridden in the config file
    m_defaultCPULoadThreshold(25), // 25% CPU-load
    m_cpuLoadPollingDelay(1),      // 1 sec delay between polling CPU-load
    m_rePrestartDelay(3600 * 3),   // 3 hours timer after screen blanks
    m_defaultRePrestartTrigger(3)  // > 3 releases leads into forced re-prestart
{}

void ConfigData::addPrestartApp(PrestartApp * app)
{
    m_prestartApps.append(app);
}

const ConfigData::AppVect & ConfigData::prestartApps() const
{
    return m_prestartApps;
}

void ConfigData::addAllowedLazyShutdownApp(const QString & path)
{
    if (!m_allowedApps.contains(path))
    {
        m_allowedApps.append(path);
    }
}

const QStringList & ConfigData::allowedLazyShutdownApps() const
{
    return m_allowedApps;
}

void ConfigData::setDefaultCPULoadThreshold(int value)
{
    m_defaultCPULoadThreshold = value;
}

int ConfigData::defaultCPULoadThreshold() const
{
    return m_defaultCPULoadThreshold;
}

void ConfigData::setDefaultRePrestartTrigger(int value)
{
    m_defaultRePrestartTrigger = value;
}

int ConfigData::defaultRePrestartTrigger() const
{
    return m_defaultRePrestartTrigger;
}

void ConfigData::setCPULoadPollingDelay(int value)
{
    m_cpuLoadPollingDelay = value;
}

int ConfigData::cpuLoadPollingDelay() const
{
    return m_cpuLoadPollingDelay;
}

void ConfigData::setRePrestartDelay(int value)
{
    m_rePrestartDelay = value;
}

int ConfigData::rePrestartDelay() const
{
    return m_rePrestartDelay;
}

