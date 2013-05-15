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

#ifndef MULTIWINDOWAPPLICATION_H
#define MULTIWINDOWAPPLICATION_H

#include <MApplication>
#include <MApplicationWindow>

#include <QSharedPointer>

#include "mainpage.h"
#include "multiwindowservice.h"

class MultiWindowApplication : public MApplication
{
public:
    static const int NUM_WINDOWS = 3;

    MultiWindowApplication(int argc, char ** argv,
        const QString &serviceName);
    virtual ~MultiWindowApplication();
    void activateWindow(int index);

    void createWindows();

protected:

    //! \reimp
    virtual void releasePrestart();

    //! \reimp
    virtual void restorePrestart();

private:
    void resetAndStopWidgets();

    MApplicationWindow *m_window[NUM_WINDOWS];
    MainPage *m_mainPage[NUM_WINDOWS];

    // D-Bus service to show a specific window. Showing a window also causes the
    // application to get released from the prestarted state.
    MultiWindowService *m_service;
};

#endif // MULTIWINDOWAPPLICATION_H
