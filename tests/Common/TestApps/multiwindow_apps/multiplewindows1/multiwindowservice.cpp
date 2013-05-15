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

#include "multiwindowservice.h"
#include "multiwindowapplication.h"
#include "multiplewindowsifadaptor.h"

#include <QDBusConnection>
#include <MDebug>

MultiWindowService::MultiWindowService(MultiWindowApplication * pApp,
    const QString &serviceName) :
    m_pApp(pApp)
{
    // Register the service
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (connection.registerService(serviceName)) {
        // Create adaptor for the service
        new MultipleWindowsIfAdaptor(this);
        // Register object
        connection.registerObject("/", this);
    } else {
        mDebug("MultiWindowService") << "Registering " << serviceName << " failed.";
        exit(-1);
    }
}

MultiWindowService::~MultiWindowService()
{}

void MultiWindowService::launchWindow(int windowId)
{
    m_pApp->activateWindow(windowId);
}

