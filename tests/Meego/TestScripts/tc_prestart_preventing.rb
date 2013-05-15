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

# TODO: test that apps started by init system are not killed
# when they go to prestarted state

require 'tdriver'
require 'test/unit'
require 'test/unit/ui/console/testrunner'

in_sb = false
daemon_running_for = 50
daemon_running = false

class TC_PrestartPreventing < Test::Unit::TestCase

    def setup()
        @sut = TDriver.sut(:Id => ARGV[0] || 'sut_qt_maemo')

        if ENV['PATH'].include?("scratchbox")
            @in_sb = true
        else
            system("mcetool --set-tklock-mode=unlocked")
            @in_sb = false
        end

        # check for how long the daemon is going to sleep, default to 50 secs
        File.open('/usr/share/applifed-M-testscripts/prevent.drive') { |f|
            f.readlines().each { |line|
                if line =~ /^EXIT_WITH_DELAY=(\d+)/i
                    @daemon_running_for = $1.to_i()
                    break
                end
            }
        }

        puts("daemon is planning to sleep for #{@daemon_running_for} " +
             "seconds ...")
        
        # stop daemon if it's running on the system
        if @in_sb
            @daemon_running = system('pkill applifed')
        else
            @daemon_running = system('initctl stop xsession/applifed')
        end
        
        sleep(2)
        
        # kill applications that are already running
        system('pkill fali')

        # start the daemon in test mode
        system("source /tmp/session_bus_address.user; " +
               "DISPLAY=#{ENV['DISPLAY']} " +
               "applifed --test --log /tmp/prevent_prestart.log " +
               "< /usr/share/applifed-M-testscripts/prevent.drive &")

        @start_time = Time.now()
        
        sleep(5)
    end
    
    def teardown()
        # sleep so that the daemon has time to exit ...
        sleep_time = @daemon_running_for - (Time.now() - @start_time) + 5
        if sleep_time > 0
            puts "sleeping for #{sleep_time} seconds ..."
            sleep(sleep_time)
        end

        system('pkill fali')

        # start the daemon again, if necessary
        if @daemon_running
            if @in_sb
                system('applifed &')
            else
                system('initctl start xsession/applifed')
            end
        end
    end

    def test_prestart_preventing()
        # prestart fali_perf
        system('dbus-send --type=method_call --dest=com.nokia.fali_perf ' +
               '/org/maemo/m com.nokia.MApplicationIf.ping')
        sleep(2)

        # show its window
        system('dbus-send --type=method_call --dest=com.nokia.fali_perf ' +
               '/org/maemo/m com.nokia.MApplicationIf.launch')
        sleep(2)

        # close the window
        app = @sut.application(:name => 'fali_perf')
        app.MButton(:name => 'CloseButton').tap()
        sleep(5)

        # verify that fali_perf was killed
        assert(system('pgrep fali_perf') == false, 'fali_perf was not killed')
    end

    def test_whitelist()
        # start an app that can lazy shutdown (and that is whitelisted ...)
        system('/usr/bin/fali_mw1 -software -prestart &')
        sleep(2)

        # show a window
        system('dbus-send --dest=com.nokia.fali_mw1 --type=method_call ' +
               '/ com.nokia.MultipleWindowsIf.launchWindow int32:1')
        sleep(2)

        # close the window
        app = @sut.application(:name => 'fali_mw1')
        app.MButton(:name => 'CloseButton').tap()
        sleep(4)
        
        # verify that the app was not killed
        assert(system('pgrep fali_mw1'), 'fali_mw1 was killed!')

        system('pkill fali_mw1')
    end
end

