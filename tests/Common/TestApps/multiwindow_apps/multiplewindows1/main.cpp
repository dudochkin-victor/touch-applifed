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
#include <MApplication>

int main(int argc, char **argv)
{
    QString appName(argv[0]);
    int retval = 1;

    if (appName.endsWith("fali_mw1"))
    {
        MultiWindowApplication app(argc, argv, "com.nokia.fali_mw1");
        MApplication::setPrestartMode(M::LazyShutdownMultiWindow);
        app.createWindows();
        retval = app.exec();
    }
    else if (appName.endsWith("fali_mw2"))
    {
        MultiWindowApplication app(argc, argv, "com.nokia.fali_mw2");
        MApplication::setPrestartMode(M::TerminateOnCloseMultiWindow);
        app.createWindows();
        retval = app.exec();
    }
    else
    {
        qWarning("Unrecognized executable name!");
    }

    return retval;
}
