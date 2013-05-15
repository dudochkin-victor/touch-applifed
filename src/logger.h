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

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>
#include <QFile>

/*!
 * \class Logger
 * \brief Logging utility class.
 *
 * Debug messages can be disabled by defining DEBUG_LOGGING_DISABLED.
 */
class Logger
{
public:

    /*!
     * \brief Open the log
     * \param progName Program name as it will be seen in the log.
     * \param fileName Name of the log file used.
     */
    static void openLog(const char * progName, const char * fileName);

    /*!
     * \brief Close the log
     */
    static void closeLog();


    /*!
     * \brief Log a notice to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logNotice(const char * format, ...);


    /*!
     * \brief Log an error to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logError(const char * format, ...);


    /*!
     * \brief Log a warning to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logWarning(const char * format, ...);


    /*!
     * \brief Log a piece of information to the system message logger
     * \param format String identical to a printf format string
     * \param additionalArgs Depending on the format string, the function may expect a 
     *        sequence of additional arguments, each containing one value to be inserted
     *        in the format parameter, if any. 
     */
    static void logInfo(const char * format, ...);

    /*!
     * \brief Forces Logger to echo everything to stdout if set to true.
     */
    static void setEchoMode(bool enable);

private:

    static void writeLog(const int priority, const char * format, va_list ap); 

    //! True if the log is open
    static bool m_isOpened;

    //! Log file which is used if the syslog is not available
    static QFile m_logFile;

    //! Text stream for log file
    static QTextStream m_logStream;

    //! True if the syslog is available
    static bool m_useSyslog; 

    //! Echo everything to stdout if true
    static bool m_echoMode;
};

#endif // LOGGER_H

