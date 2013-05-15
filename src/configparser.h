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

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QtXml>

#include "configdata.h"

/*! \class ConfigParser
 *
 * ConfigParser is an XML-parser for Applifed's config file.
 * Example config file:
 *
 * <?xml version="1.0"?>
 * <applifed default_cpu_load_threshold="50" cpu_load_polling_delay="2" re-prestart_delay="3600"
 *   default_re-prestart_trigger="5">
 *   <application name="Foo" service="org.maemo.Foo" priority="0" cpu_load_threshold="25"/>
 *   <application name="Bar" service="org.maemo.Bar" priority="-1"/>
 *   <application name="Humppa" service="org.maemo.Humppa" priority="-1" re-prestart_trigger="3"/>
 *   <application name="Delme" service="org.maemo.Delme" priority="2" forced_re-prestart_not_allowed="1"/>
 *   <allow_lazy_shutdown binary_path="/usr/bin/application1"/>
 *   <allow_lazy_shutdown binary_path="/usr/bin/application2"/>
 * </applifed>
 *
 */
class ConfigParser : public QXmlDefaultHandler
{
public:

    //! Constructor
    ConfigParser() {};

    //! \reimp
    virtual bool startDocument();

    //! \reimp
    virtual bool endDocument();

    //! \reimp
    virtual bool startElement(const QString &, const QString &, const QString &, const QXmlAttributes &);

    //! \reimp
    virtual bool endElement(const QString &, const QString &, const QString &);

    //! \reimp
    virtual bool fatalError(const QXmlParseException & exception);

    //! Return data
    const ConfigData & data() const;

private:

    //! Disable copy constructor
    Q_DISABLE_COPY(ConfigParser);

    //! Print element error to stderr
    void elementError(const QString & qName);

    ConfigData m_data;
};

#endif // CONFIGPARSER_H
