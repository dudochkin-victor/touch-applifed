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

#ifndef MULTIWINDOWSERVICE_H
#define MULTIWINDOWSERVICE_H

#include <QObject>

class MultiWindowApplication;

class MultiWindowService : public QObject
{
    Q_OBJECT

public:
    MultiWindowService(MultiWindowApplication * pApp,
        const QString &serviceName);
    virtual ~MultiWindowService();

public Q_SLOTS:
    void launchWindow(int windowId);

private:
    MultiWindowApplication * m_pApp;
};

#endif // MULTIWINDOWSERVICE_H
