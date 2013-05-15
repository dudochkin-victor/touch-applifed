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

#include <QTimer>
#include <MLayout>
#include <MButton>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MProgressIndicator>
#include <MDebug>

#include "mainpage.h"

MainPage::MainPage(QString windowName) :
    m_windowName(windowName)
{
    setTitle(windowName);

    m_layout = new MLayout(centralWidget());
    m_policy = new MGridLayoutPolicy(m_layout);
    m_policy->setSpacing(20.0);

    m_label1 = new MLabel(m_windowName);
    m_label1->setAlignment(Qt::AlignCenter);
    m_policy->addItem(m_label1, 0, 0);

    m_bar1 = new MProgressIndicator(NULL, MProgressIndicator::barType);
    m_bar1->setRange(0, 100);

    m_policy->addItem(m_bar1, 1, 0);
    m_layout->setLandscapePolicy(m_policy);

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateBar()));
}

MainPage::~MainPage()
{
    delete m_policy;
    m_policy = NULL;
}

void MainPage::createContent()
{}

void MainPage::updateBar()
{
    m_bar1->setValue(m_bar1->value() + 1);
}

void MainPage::stopAndResetWidgets()
{
    m_timer->stop();
    m_bar1->reset();

    mDebug("MainPage") << "Prestart restored";
}

void MainPage::activateWidgets()
{
    m_timer->start();

    mDebug("MainPage") << "Prestart released";
}
