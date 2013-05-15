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

#include "configparser.h"
#include "logger.h"

bool ConfigParser::startDocument()
{
    return true;
}

bool ConfigParser::endDocument()
{
    return true;
}

bool ConfigParser::startElement(const QString & namespaceURI,
                                const QString & localName,
                                const QString & qName,
                                const QXmlAttributes & atts)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == "applifed")
    {
        int index = atts.index("default_cpu_load_threshold");
        if (index != -1)
        {
            m_data.setDefaultCPULoadThreshold(atts.value(index).toInt());
        }

        index = atts.index("cpu_load_polling_delay");
        if (index != -1)
        {
            m_data.setCPULoadPollingDelay(atts.value(index).toInt());
        }

        index = atts.index("re-prestart_delay");
        if (index != -1)
        {
            m_data.setRePrestartDelay(atts.value(index).toInt());
        }

        index = atts.index("default_re-prestart_trigger");
        if (index != -1)
        {
            m_data.setDefaultRePrestartTrigger(atts.value(index).toInt());
        }
    }
    else if (qName == "allow_lazy_shutdown")
    {
        int index = atts.index("binary_path");
        if (index != -1)
        {
            m_data.addAllowedLazyShutdownApp(atts.value(index));
        }
    }
    else if (qName == "application")
    {
        QString name, service;

        int priority                   =  0;
        int rePrestartTrigger          =  0;
        int cpuLoadThreshold           = -1;
        int forcedRePrestartNotAllowed = -1;

        int index = atts.index("name");
        if (index != -1)
        {
            name = atts.value(index);
        }
        else
        {
            elementError(qName);
            return false;
        }

        index = atts.index("service");
        if (index != -1)
        {
            service = atts.value(index);
        }
        else
        {
            elementError(qName);
            return false;
        }

        index = atts.index("cpu_load_threshold");
        if (index != -1)
        {
            cpuLoadThreshold = atts.value(index).toInt();
        }

        index = atts.index("priority");
        if (index != -1)
        {
            priority = atts.value(index).toInt();
        }

        index = atts.index("re-prestart_trigger");
        if (index != -1)
        {
            rePrestartTrigger = atts.value(index).toInt();
        }

        index = atts.index("forced_re-prestart_not_allowed");
        if (index != -1)
        {
            forcedRePrestartNotAllowed = atts.value(index).toInt();
        }

        // Create a new app structure
        PrestartApp * app = new PrestartApp(name, service, priority);

        // Set default value first, then override with custom value
        // if it was set
        app->setCPULoadThreshold(m_data.defaultCPULoadThreshold());
        if (cpuLoadThreshold > -1)
        {
            app->setCPULoadThreshold(cpuLoadThreshold);
        }

        if (forcedRePrestartNotAllowed > -1)
        {
            app->setForcedRePrestartNotAllowed(forcedRePrestartNotAllowed);
        }

        // Set default value first, then override with custom value
        // if it was set
        app->setRePrestartTrigger(m_data.defaultRePrestartTrigger());
        if (rePrestartTrigger > 0)
        {
            app->setRePrestartTrigger(rePrestartTrigger);
        }

        m_data.addPrestartApp(app);
    }
    else
    {
        Logger::logWarning("ConfigParser: Ignoring unknown element '%s'",
                           qName.toStdString().c_str());
    }

    return true;
}

bool ConfigParser::endElement(const QString &, const QString &, const QString &)
{
    return true;
}

bool ConfigParser::fatalError(const QXmlParseException & exception)
{
    Logger::logError("ConfigParser: Fatal error on line %d, column %d: '%s'",
                     exception.lineNumber(),
                     exception.columnNumber(),
                     exception.message().toStdString().c_str());
    return false;
}

void ConfigParser::elementError(const QString & qName)
{
    Logger::logError("ConfigParser: Syntax error in element '%s'", qName.toStdString().c_str());
}

const ConfigData & ConfigParser::data() const
{
    return m_data;
}
