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

#ifndef X11UTIL_H
#define X11UTIL_H

#include <QX11Info>
#include <QVector>
#include <QString>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

//! X11 helper class
class X11Util
{
public:
    //! Return corresponding PID of an X11 window. Return -1 if fails.
    static int windowPid(Window window);

    //! Return list of windows corresponding to the given PID
    static QVector<Window> windowsForPid(int pid);

    //! Return client list of windows
    static QVector<Window> clientList();

    //! Return corresponding application binary name of an X11 window. Return empty string if fails.
    static QString applicationName(Window window);

    // Return boolean value True / False if XProperty _MEEGOTOUCH_PRESTARTED is set / unset.
    static bool isPrestarted(Window window);
};

#endif // X11UTIL_H
