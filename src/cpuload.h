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

#ifndef CPULOAD_H
#define CPULOAD_H

#include <QList>
#include <QByteArray>

/*! \class CPULoad
 *  \brief CPULoad calculates the current CPU load.
 */
class CPULoad
{
public:

    //! Constructor
    CPULoad();

    /*! \brief Initialize. Currently only sets the value to -1.
     */
    void init();

    /*! \brief Update the value.
     * This must be called periodically (e.g. 1 - 5 seconds) in order to update the measurement.
     * The data is obtained from /proc/stat.
     */
    void update();

    /*! \returns The current value of the load in the range of 0 to 100.
     *  \returns -1 if nothing measured.
     */
    int getValue() const;

    /*! \brief Force the CPU-load value to the given value.
     * This method is for testing purposes. The forced value
     * is active if > -1.
     */
    void forceValue(int forcedValue);

private:

    //! Disable copy constructor
    CPULoad(const CPULoad &);

    //! Disable assignment
    CPULoad & operator= (const CPULoad &);

    //! Get a new time list from /proc/stat
    typedef QList<QByteArray> TimeList;
    TimeList getTimeList();

    //! Current time list
    TimeList m_timeList;

    //! Current load value -1..100
    int m_load;

    //! If > -1, return this instead of the real value.
    //! For functional testing purposes.
    static int m_forcedLoad;

#ifdef UNIT_TEST
    friend class Ut_CPULoad;
#endif
};

#endif // CPULOAD_H
