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

#include "multiplewindowsifadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class MultipleWindowsIfAdaptor
 */

MultipleWindowsIfAdaptor::MultipleWindowsIfAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

MultipleWindowsIfAdaptor::~MultipleWindowsIfAdaptor()
{
    // destructor
}

void MultipleWindowsIfAdaptor::launchWindow(int windowId)
{
    // handle method call com.nokia.MultipleWindowsIf.launchWindow
    QMetaObject::invokeMethod(parent(), "launchWindow", Q_ARG(int, windowId));
}

