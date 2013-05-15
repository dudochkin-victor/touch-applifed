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

#include "applifeservice.h"
#include "applifeservice_p.h"
#include "applifedifadaptor.h"
#include "daemon.h"
#include "prestarter.h"
#include "global.h"
#include "logger.h"

#include <QDebug>

#include <QDBusConnection>

AppLifeServicePrivate::AppLifeServicePrivate(Daemon * daemon):
    m_isRegistered(false),
    m_daemon(daemon),
    q_ptr(NULL),
    m_dBusConnection(QDBusConnection::sessionBus())
{}

bool AppLifeServicePrivate::registerService(const QString &serviceName)
{
    return m_dBusConnection.registerService(serviceName);
}

void AppLifeServicePrivate::registerObject(const QString &path, QObject *object)
{
    m_dBusConnection.registerObject(path, object);
}

void AppLifeServicePrivate::unregisterObject(const QString &path)
{
    m_dBusConnection.unregisterObject(path);
}

AppLifeServicePrivate::~AppLifeServicePrivate()
{}

AppLifeService::AppLifeService(Daemon * daemon, QObject *parent):
    d_ptr(new AppLifeServicePrivate(daemon))
{
    Q_UNUSED(parent);

    Q_D(AppLifeService);
    d->q_ptr = this;
}

void AppLifeService::prestartRestored(int pid)
{
    Q_D(AppLifeService);

    d->m_daemon->prestarter()->restoredPrestart(pid);
}

bool AppLifeService::registerService()
{
    Q_D(AppLifeService);

    if (!d->m_isRegistered)
    {
        // Generate serve name
        QString serviceName = "com.nokia." + QString(PROG_NAME);

        // Instantiate interface adaptor
        new ApplifedIfAdaptor(this);

        // Register the service
        if (d->registerService(serviceName))
        {
            // Register the object
            d->unregisterObject("/org/maemo/applifed");
            d->registerObject("/org/maemo/applifed", this);
            d->m_isRegistered = true;
        }
        else
        {
            return false;
        }
   }

    return true;
}

AppLifeService::~AppLifeService()
{
    delete d_ptr;
}
