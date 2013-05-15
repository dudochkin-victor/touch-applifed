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

#ifndef PRESTARTPARSER_H
#define PRESTARTPARSER_H

class QTextStream;
class QString;
class PrestartApp;

//! Parser for prestart config files
namespace PrestartParser
{
    //! Read data from the given file
    PrestartApp * parsePrestartFile(const QString & filePath);

    //! Read data using the given stream
    PrestartApp * parsePrestartData(QTextStream & stream);
}

#endif // PRESTARTPARSER_H
