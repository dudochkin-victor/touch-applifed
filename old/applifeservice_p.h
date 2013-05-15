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

#ifndef APPLIFESERVICE_P_H
#define APPLIFESERVICE_P_H

#include <QString>
#include <QDBusConnection>

class AppLifeService;
class Daemon;

class AppLifeServicePrivate
{
    Q_DECLARE_PUBLIC(AppLifeService)

public:
    AppLifeServicePrivate(Daemon * daemon);
    virtual ~AppLifeServicePrivate();

    bool m_isRegistered;
    Daemon * m_daemon;

protected:
    AppLifeService * q_ptr;

    bool registerService( const QString &serviceName );
    void registerObject( const QString &path, QObject *object );
    void unregisterObject( const QString &path );

private:
    QDBusConnection m_dBusConnection;
};

#endif // APPLIFESERVICE_P_H
