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

#include "prestartparser.h"
#include "prestartapp.h"

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>

PrestartApp * PrestartParser::parsePrestartFile(const QString & filePath)
{
    // Open the given conf file
    QFile fin(filePath);
    if (fin.open(QFile::ReadOnly))
    {
        // Stream from the file
        QTextStream stream(&fin);
        return PrestartParser::parsePrestartData(stream);
    }

    return NULL;
}

PrestartApp * PrestartParser::parsePrestartData(QTextStream & stream)
{
    // Service name
    QString service;
    // Current line
    QString line;
    // Current line number
    uint lineNumber = 1;
    // Prestart priority
    int priority = 0;

    // Read the stream line by line
    do
    {
        // Process the line if it's not empty and
        // doesn't start with "#"
        line = stream.readLine();
        if (line != "" && !line.startsWith("#"))
        {
            // Parse the application service
            if (line.startsWith("Service="))
            {
                QStringList splitLine = line.split('=');
                if (splitLine.size() == 2)
                {
                    service = splitLine.at(1);
                }
            }
            else if (line.startsWith("Priority="))
            {
                QStringList splitLine = line.split('=');
                if (splitLine.size() == 2)
                {
                    priority = splitLine.at(1).toInt();
                }
            }
        }

        // Increment the line number
        lineNumber++;

    } while (!line.isNull());

    // Check that service was defined
    if (service.size())
    {
        // Create a new prestart app structure and return it
        return new PrestartApp(service, priority);
    }

    return NULL;
}

