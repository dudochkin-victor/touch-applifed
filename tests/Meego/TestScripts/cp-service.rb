#!/usr/bin/ruby
#
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# Contact: Nokia Corporation (directui@nokia.com)
#
# This file is part of applifed.
#
# If you have questions regarding the use of this file, please contact
# Nokia at directui@nokia.com.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License version 2.1 as published by the Free Software Foundation
# and appearing in the file LICENSE.LGPL included in the packaging
# of this file.
#

$path = string = `echo $PATH `
if $path.include?("scratchbox")
    system("cp /usr/share/applifed-testscripts/sbservice/com.nokia.fali_hello.service.sb /usr/share/dbus-1/services/com.nokia.fali_hello.service")
    system("cp /usr/share/applifed-testscripts/sbservice/com.nokia.fali_toc.service.sb /usr/share/dbus-1/services/com.nokia.fali_toc.service")
end
