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

#include "logger.h"

#include <syslog.h>
#include <cstdarg>
#include <QDateTime>
#include <QDir>
#include <QString>

namespace
{
    const QString dateFormat("yyyy-MM-dd hh:mm:ss.zzz");
}

bool Logger::m_isOpened  = false;
bool Logger::m_useSyslog = false;
bool Logger::m_echoMode  = false;

QTextStream Logger::m_logStream;
QFile Logger::m_logFile;

void Logger::openLog(const char * progName, const char * fileName)
{
    // Check that log is not already opened
    if (!Logger::m_isOpened)
    {
        // Log to file if syslog is not wanted
        if (!m_useSyslog)
        {
            // Open log file as a QTextStream
            m_logFile.setFileName(fileName);
            if (m_logFile.open(QIODevice::WriteOnly))
            {
                Logger::m_logStream.setDevice(&m_logFile);
            }
        }
        // Log to syslog
        else
        {
            // Open syslog
            openlog(progName, LOG_PID, LOG_DAEMON);
        }

        Logger::m_isOpened = true;
    }
}

void Logger::closeLog()
{
    // Check that log is opened
    if (Logger::m_isOpened)
    {
        // Close syslog
        if (m_useSyslog)
        {
            closelog();
        }
        // Close log file
        else
        {
            m_logFile.close();
        }

        Logger::m_isOpened = false;
    }
}

void Logger::writeLog(const int priority, const char * format, va_list ap)
{
    // Check that log is opened
    if (Logger::m_isOpened)
    {
        // In echo mode always print everything to stdout
        if (m_echoMode)
        {
            vprintf(format, ap);
            printf("\n");
        }

        // Log to syslog if used
        if (m_useSyslog)
        {
            vsyslog(priority, format, ap);
        }
        // Log to file
        else
        {
            // Create a QString from the (const char *) message
            QString msg;
            msg.vsprintf(format, ap);

            // Send current date and time to the stream
            m_logStream << 
                QDateTime::currentDateTime().toString(dateFormat);

            // Send correct prefix to the stream
            switch (priority)
            {
            case LOG_NOTICE:
                m_logStream << " [NOTICE] ";
                break;
            case LOG_ERR:
                m_logStream << " [ERROR] ";
                break;
            case LOG_WARNING:
                m_logStream << " [WARNING] ";
                break;
            case LOG_INFO:
                m_logStream << " [INFO] ";
                break;
            default:
                m_logStream << " [N/A] ";
                break;
            }

            // Send message to the stream
            m_logStream << msg << "\n";

            // Flush the stream
            m_logStream.flush();
        }
    }
}

void Logger::logNotice(const char * format, ...)
{
#ifndef DEBUG_LOGGING_DISABLED
    va_list(ap);
    va_start(ap, format);
    writeLog(LOG_NOTICE, format, ap);
    va_end(ap);
#else
    Q_UNUSED(format);
#endif
}

void Logger::logError(const char * format, ...)
{
    va_list(ap);
    va_start(ap, format);
    writeLog(LOG_ERR, format, ap); 
    va_end(ap);
}

void Logger::logWarning(const char * format, ...)
{
#ifndef DEBUG_LOGGING_DISABLED
    va_list(ap);
    va_start(ap, format);
    writeLog(LOG_WARNING, format, ap); 
    va_end(ap);
#else
    Q_UNUSED(format);
#endif
}

void Logger::logInfo(const char * format, ...)
{
#ifndef DEBUG_LOGGING_DISABLED
    va_list(ap);
    va_start(ap, format);
    writeLog(LOG_INFO, format, ap); 
    va_end(ap);
#else
    Q_UNUSED(format);
#endif
}

void Logger::setEchoMode(bool enable)
{
    Logger::m_echoMode = enable;
}

