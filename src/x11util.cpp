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

#include "x11util.h"

static int handleXError(Display *, XErrorEvent *)
{
    return 0;
}

int X11Util::windowPid(Window window)
{
    int pid = -1;

    Display * dpy  = QX11Info::display();
    if (dpy)
    {
        Atom           pidAtom = XInternAtom(dpy, "_NET_WM_PID", False);
        Atom           type;
        int            format;
        unsigned long  nItems;
        unsigned long  bytesAfter;
        unsigned char *propPID = 0;

        int (*previousHandler)(Display *, XErrorEvent *);
        previousHandler = XSetErrorHandler(handleXError);

        // Get the PID of the window
        if(XGetWindowProperty(dpy, window, pidAtom, 0, 1, False, XA_CARDINAL,
                              &type, &format, &nItems, &bytesAfter, &propPID) == Success)
        {
            if(propPID != 0)
            {
                // If the PID matches, add this window to the result set.
                pid = *(reinterpret_cast<int *>(propPID));
                XFree(propPID);
            }
        }

        XSetErrorHandler(previousHandler);
    }

    return pid;
}

QString X11Util::applicationName(Window window) 
{
    QString ret   = "";
    Display * dpy = QX11Info::display();

    if (dpy)
    {
        char **argv_return = 0;
        int argc_return = 0;

        int (*previousHandler)(Display *, XErrorEvent *);
        previousHandler = XSetErrorHandler(handleXError);

        int result = XGetCommand(dpy, window, &argv_return, &argc_return);
        if (result >= Success && argc_return && *argv_return)
        {
            ret = QString(*argv_return);
        }
       
        if (argv_return) 
        {
            XFreeStringList(argv_return);
        }

        XSetErrorHandler(previousHandler);
    }

    return ret;
}

bool X11Util::isPrestarted(Window window) 
{
    bool ret       = False;
    Display * dpy  = QX11Info::display();

    if (dpy)
    {
        Atom           prestartAtom = XInternAtom(dpy, "_MEEGOTOUCH_PRESTARTED", False);
        Atom           type;
        int            format;
        unsigned long  nItems;
        unsigned long  bytesAfter;
        unsigned char *prestarted = 0;

        int (*previousHandler)(Display *, XErrorEvent *);
        previousHandler = XSetErrorHandler(handleXError);

        if(XGetWindowProperty(dpy, window, prestartAtom, 0, 1, False, XA_CARDINAL,
                              &type, &format, &nItems, &bytesAfter, &prestarted) == Success)
        {
            if(prestarted)
            {
                ret = True;
                XFree(prestarted);
            }
        }

        XSetErrorHandler(previousHandler);
    }

    return ret;
}


QVector<Window> X11Util::windowsForPid(int pid)
{
    const QVector<Window> clients(clientList());
    QVector<Window> result;

    Q_FOREACH(Window window, clients)
    {
        if (windowPid(window) == pid)
        {
            result << window;
        }
    }

    return result;
}

QVector<Window> X11Util::clientList()
{
    QVector<Window> clients;
    Display * dpy = QX11Info::display();
    if (dpy)
    {
        Atom            clientListAtom = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
        Atom            actualType;
        int             actualFormat;
        unsigned long   numWindowItems;
        unsigned long   bytesLeft;
        unsigned char * windowData = NULL;

        int (*previousHandler)(Display *, XErrorEvent *);
        previousHandler = XSetErrorHandler(handleXError);

        int result = XGetWindowProperty(dpy, DefaultRootWindow(dpy),
                                        clientListAtom, 0, 0x7fffffff,
                                        False, XA_WINDOW,
                                        &actualType, &actualFormat,
                                        &numWindowItems, &bytesLeft, &windowData);

        if (result == Success && windowData != None)
        {
            Window * wins = reinterpret_cast<Window *>(windowData);
            if (wins)
            {
                XWindowAttributes wAttributes;
                for (unsigned int i = 0; i < numWindowItems; i++)
                {
                    // Filter windows that are:
                    // - bigger than 0x0 are Input/Output windows
                    // - are not unmapped
                    if (XGetWindowAttributes(dpy, wins[i], &wAttributes) != 0 &&
                        wAttributes.width > 0 && wAttributes.height > 0 &&
                        wAttributes.c_class == InputOutput &&
                        wAttributes.map_state != IsUnmapped)
                    {
                        clients << wins[i];
                    }
                }

                XFree(wins);
            }
        }

        XSetErrorHandler(previousHandler);
    }

    return clients;
}

