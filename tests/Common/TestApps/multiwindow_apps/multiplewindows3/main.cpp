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

#include <QProcess>
#include <QStringList>

#include <MApplication>
#include <MApplicationService>
#include <MApplicationIfAdaptor>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MAction>
#include <MLocale>
#include <MDebug>
#include <signal.h>

#include "mainpage.h"
#include "multiwindowservice.h"
#include "multiplewindowsifadaptor.h"

int main(int argc, char **argv)
{
    MApplication app(argc, argv);
    MApplication::setPrestartMode(M::LazyShutdownMultiWindow);

    // Create an own D-Bus service to allow the launching of the desired window
    MultiWindowService service;
    new MultipleWindowsIfAdaptor(&service);

    // If the application is prestarted, connect signals to set and reset
    // widgets in case of a prestart restored or released.

    if(app.isPrestarted()) {
        app.connect(&app, SIGNAL(prestartReleased()), &service, SLOT(activateWidgets()));
        app.connect(&app, SIGNAL(prestartRestored()), &service, SLOT(deactivateWidgets()));
    }
 
    return app.exec();
}


