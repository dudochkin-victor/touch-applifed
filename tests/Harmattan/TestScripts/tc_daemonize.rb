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

ADDITIONAL_PATH=File.join("/usr/share/applifed-testscripts")
$LOAD_PATH << ADDITIONAL_PATH

require 'fautils'

class TC_Daemonize < Test::Unit::TestCase
    $path = string = `echo $PATH `
    LAUNCH_APPLIFED_COUNT = 5
    def setup
        @tc = Fali_utils.new
        @tc.get_pids('applifed')
        if $path.include?("scratchbox")
            system("pkill applifed")
            system("rm /tmp/applifed*")
        end
	    system("initctl stop xsession/applifed")
    end
    
    def teardown
        if $path.include?("scratchbox")
	    system("applifed --fork")
	else
	    system("rm /tmp/applifed*")
	    system("pkill applifed")
	    system("initctl start xsession/applifed")
	end
        @tc.get_pids('applifed')
    end
    def test_daemonize()
        @sut = TDriver.sut(:Id => 'sut_qt_maemo')
        @tc.run_system_command(:cmd => "applifed --fork", \
                               :failmsg => "error while daemonizing", \
                               :docheck => true, :sleep => 5, \
                               :logmsg => "starting applifed daemonized")

        @tc.run_system_command(:cmd => "pgrep applifed", \
                               :failmsg => "applifed was not started", \
                               :docheck => true, :sleep => 1, \
                               :logmsg => "checking for applifed")

        pids = @tc.get_pids('applifed')
        if not pids.nil?
            @tc.print_debug("pids: #{pids.join(' ')}")
        else
            verify_true(0, "Applifed is not running") { false }
        end
        
        verify_true(0, "Multiple instances of applifed running") {
               pids.length == 1}

        @tc.run_system_command(:cmd => "dbus-send --dest=com.nokia.fali_hello"+\
                               " --type=method_call /org/maemo/m com.nokia.MApplicationIf.ping", \
                               :failmsg => "unable to prestart application", \
                               :docheck => true, :sleep => 1, \
                               :logmsg => "prestarting fali_hello")

        @tc.run_system_command(:cmd => "dbus-send --dest=com.nokia.fali_hello"+\
                               " --type=method_call /org/maemo/m com.nokia.MApplicationIf.launch", \
                               :failmsg => "unable to launch application", \
                               :docheck => true, :sleep => 5, \
                               :logmsg => "starting fali_hello")

        @tc.run_system_command(:cmd => "pgrep fali_hello", \
                               :failmsg => "fali_hello was not started", \
                               :docheck => true, :sleep => 1, \
                               :logmsg => "checking for fali_hello")

        @tc.run_system_command(:cmd => "pkill applifed", \
                               :failmsg => "unable to kill applifed", \
                               :docheck => true, :sleep => 1, \
                               :logmsg => "kill applifed now")

        @tc.run_system_command(:cmd => "pkill fali_hello", \
                               :failmsg => "unable to kill fali_hello", \
                               :docheck => true, :sleep => 1, \
                               :logmsg => "kill fali_hello now")
    end
    
    def test_fork_one_instance
        for i in 1..LAUNCH_APPLIFED_COUNT
            @tc.run_system_command(:cmd => "applifed --fork", \
                    :failmsg => "error while forking", \
                    :sleep => 1, \
                    :logmsg => "Forking applifed #{i} times")
        end
        pids = @tc.get_pids('applifed')
        if not pids.nil?
            @tc.print_debug("pids: #{pids.join(' ')}")
        else
            verify_true(0, "Applifed is not running") { false }
        end
        
        verify_true(0, "Multiple instances of applifed running") {
               pids.length == 1}
    end
end


