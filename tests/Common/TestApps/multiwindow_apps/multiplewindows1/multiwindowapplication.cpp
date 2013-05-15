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

#include "multiwindowapplication.h"
#include "multiwindowservice.h"
#include "mainpage.h"

#include <MDebug>

MultiWindowApplication::MultiWindowApplication(int argc, char ** argv,
    const QString &serviceName) :
    MApplication(argc, argv),
    m_service(new MultiWindowService(this, serviceName))
{
}

void MultiWindowApplication::createWindows()
{
    // Create windows
    for (int i = 0; i < NUM_WINDOWS; i++) {
        m_window[i] = new MApplicationWindow;
        m_window[i]->setWindowTitle(QString("Window %1").arg(i + 1));
        m_window[i]->setObjectName(QString("Window %1").arg(i + 1));
    }

    // Create pages
    for (int i = 0; i < NUM_WINDOWS; i++)
        m_mainPage[i] = new MainPage(QString("Window %1").arg(i + 1));
}

void MultiWindowApplication::activateWindow(int index)
{
    index--;
    if (index >= 0 && index < NUM_WINDOWS)  {

        // Force release from prestart if in prestarted state.
        // This is important.
        if (isPrestarted()) {
            setPrestarted(false);
        }

        // Show the desired window
        m_window[index]->show();
        m_window[index]->activateWindow();
        m_window[index]->raise();

        // Show the page and activate it. Note: we have to specify on
        // what window the page is to appear. Apparently there's something
        // funny going on somewhere far far away, because the window
        // we think is active might really not be active at this point of time.
        // So if the window is not specified, the page might actually
        // end up in the previous window.
        m_mainPage[index]->appear(m_window[index]);
        m_mainPage[index]->activateWidgets();
    }
}

void MultiWindowApplication::releasePrestart()
{
    mDebug("MultiWindowApplication") << "Prestart released";
}

void MultiWindowApplication::restorePrestart()
{
    resetAndStopWidgets();

    mDebug("MultiWindowApplication") << "Prestart restored";
}

void MultiWindowApplication::resetAndStopWidgets()
{
    // Reset and stop all widgets of all pages here
    for (int i = 0; i < NUM_WINDOWS; i++)
        m_mainPage[i]->stopAndResetWidgets();
}

MultiWindowApplication::~MultiWindowApplication()
{}

