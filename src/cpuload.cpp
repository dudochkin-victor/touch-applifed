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

#include <QFile>
#include "cpuload.h"

CPULoad::CPULoad() :
m_load(-1)
{}

int CPULoad::m_forcedLoad = -1;

CPULoad::TimeList CPULoad::getTimeList()
{
    QFile fin("/proc/stat");
    if (fin.open(QIODevice::ReadOnly | QIODevice::Text))
        return fin.readLine().split(' ');
    return TimeList();
}

void CPULoad::init()
{
    m_load = -1;
}

void CPULoad::update()
{
    // Read timing from the file
    TimeList v(getTimeList());
    if (m_timeList.length())
    {
        // Take delta with the previous timing
        int sum  = 0;
        int idle = 0;
        for (int i = 2; i < 6 && i < v.length(); i++)
            sum += v.at(i).toInt() - m_timeList.at(i).toInt();
        idle = v.at(5).toInt() - m_timeList.at(5).toInt();

        // Calculate load
        m_load = 100.0 - (100.0 * idle / sum);
    }
    else
    {
        m_load = -1;
    }

    // Store the current timing
    m_timeList = v;
}

int CPULoad::getValue() const
{
    return m_forcedLoad > -1 ? m_forcedLoad : m_load;
}

void CPULoad::forceValue(int forcedValue)
{
    m_forcedLoad = forcedValue;
}
