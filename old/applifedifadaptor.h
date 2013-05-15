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

#ifndef APPLIFEDIFADAPTOR_H_1266242980
#define APPLIFEDIFADAPTOR_H_1266242980

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface com.nokia.ApplifedIf
 */
class ApplifedIfAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.ApplifedIf")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.nokia.ApplifedIf\">\n"
"    <method name=\"prestartRestored\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"pid\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    ApplifedIfAdaptor(QObject *parent);
    virtual ~ApplifedIfAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void prestartRestored(int pid);
Q_SIGNALS: // SIGNALS
};

#endif
