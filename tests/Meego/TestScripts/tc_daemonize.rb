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

require 'date'
require 'test/unit'
require 'test/unit/testsuite'
require 'test/unit/ui/console/testrunner'

class TC_Daemonize < Test::Unit::TestCase
    def test_daemonize()
        puts "starting applifed daemonized"
        assert(system("applifed --fork"), "error while daemonizing")

        puts "sleeping ..."
        sleep(5)

        assert(system("pgrep applifed"), "applifed was not started")

        puts "trying to start fali_hello ..."
        system("dbus-send --dest=com.nokia.fali_hello --type=method_call " +
               "/org/maemo/m com.nokia.MApplicationIf.launch")

        puts "sleeping ..."
        sleep(5)

        assert(system("pgrep fali_hello"), "fali_hello was not started")

        system("pkill applifed")
        system("pkill fali_hello")

        if system("pgrep applifed")
            system("pkill -9 applifed")
            flunk("applifed not terminated, sent SIGKILL")
        end
    end
end

scratchbox = false

if ENV['PATH'].include?('scratchbox') then
    scratchbox = true
end

if not scratchbox then
    system("initctl stop xsession/applifed")
end

system("pkill applifed")
system("rm /tmp/applifed*")

Test::Unit::UI::Console::TestRunner.run(TC_Daemonize)

if not scratchbox then
    system("rm /tmp/applifed*")
    system("initctl start xsession/applifed")
else
    system("applifed --fork")
end
