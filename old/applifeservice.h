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

#ifndef APPLIFESERVICE_H
#define APPLIFESERVICE_H

#include <QObject>

class AppLifeServicePrivate;
class Daemon;

/*!
 * \class AppLifeService
 * \brief Application Life-cycle QDBus service
 *
 */
class AppLifeService : public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief Construct AppLifeService object with service name and \a parent
     * \param daemon Main Daemon entity
     * \param parent optional parent.
     */
    AppLifeService(Daemon * daemon, QObject *parent = 0);

    /*! 
     * \brief Destroy AppLifeService object
     */
    virtual ~AppLifeService();

public Q_SLOTS: // METHODS

    /*!
     * \brief Notify applifed that application has returned to
     *        the prestarted state (lazy shutdown)
     * \param pid Pid of the application
     */
    virtual void prestartRestored(int pid);

    /*!
     * \brief Registers the sevice to the session bus
     */
    virtual bool registerService();

Q_SIGNALS:

    /*!
     * \brief A signal sent when an app returns to the prestarted state
     * \param pid PID of the application process
     */
    void applicationRestoredPrestart(int pid);

protected:
    //! d-pointer to the private class
    AppLifeServicePrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(AppLifeService)
    Q_DISABLE_COPY(AppLifeService)
};

#endif // APPLIFESERVICE_H

