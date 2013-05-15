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
#include <MLocale>
#include <MLayout>
#include <MButton>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MFlowLayoutPolicy>
#include <MProgressIndicator>
#include <QDebug>
#include <QFont>

#include "mainpage.h"

MainPage::MainPage(QString windowName) :
    m_windowName(windowName),
    m_label1(NULL)
{
    setTitle(windowName);
}

MainPage::~MainPage()
{
}

void MainPage::createContent()
{
    MLayout *layout = new MLayout(centralWidget());
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);
    policy->setSpacing(20.0);

    m_label1 = new MLabel("Fresh " + m_windowName);
    QFont font("Arial");
    font.setPixelSize(72);
    m_label1->setFont(font);
    m_label1->setAlignment(Qt::AlignCenter);
    policy->addItem(m_label1, 0, 0);

    MButton *button = new MButton("Modify content");
    connect(button, SIGNAL(clicked()), this, SLOT(buttonPress()));
    policy->addItem(button, 1, 0);

    layout->setLandscapePolicy(policy);
}

// Button pressed, modify windows content
void MainPage::buttonPress()
{
    m_label1->setColor(QColor(255, 255, 0, 255));
    m_label1->setText("Old "+ m_windowName);
}

void MainPage::deactivateWidgets()
{
    qDebug() << "Prestart restored";
    if(m_label1) {
        m_label1->setColor(QColor(0, 0, 0, 255));
        m_label1->setText("Fresh " + m_windowName);
    }

}

void MainPage::activateWidgets()
{
    qDebug() << "Prestart released";
}

