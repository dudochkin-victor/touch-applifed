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

#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <QVector>
#include <QStringList>

#include "prestartapp.h"

/*! \class ConfigData
 *  \brief ConfigData stores the data read from the config xml.
 */
class ConfigData
{
public:

    //! Constructor
    ConfigData();

    //! Add a new prestart app
    void addPrestartApp(PrestartApp * app);

    //! Return prestart apps
    typedef QVector<PrestartApp *> AppVect;
    const AppVect & prestartApps() const;

    /*! Add a new lazily shutdownable application not prestarted by applifed
     *  (by default allowed for all PrestartApp's).
     */
    void addAllowedLazyShutdownApp(const QString & path);

    //! Return allowed external lazy shutdowns
    const QStringList & allowedLazyShutdownApps() const;

    //! Set default CPU-load threshold
    void setDefaultCPULoadThreshold(int value);

    //! Return default CPU-load threshold
    int defaultCPULoadThreshold() const;

    //! Set default forced re-prestart trigger
    void setDefaultRePrestartTrigger(int value);

    //! Return default forced re-prestart trigger
    int defaultRePrestartTrigger() const;

    //! Set CPU-load polling delay in seconds
    void setCPULoadPollingDelay(int value);

    //! Return CPU-load polling delay
    int cpuLoadPollingDelay() const;

    //! Set re-prestart delay after screen blank in seconds
    void setRePrestartDelay(int value);

    //! Return re-prestart delay after screen blank
    int rePrestartDelay() const;

private:

    AppVect m_prestartApps;
    QStringList m_allowedApps;
    int m_defaultCPULoadThreshold;
    int m_cpuLoadPollingDelay;
    int m_rePrestartDelay;
    int m_defaultRePrestartTrigger;
};

#endif // CONFIGDATA_H
