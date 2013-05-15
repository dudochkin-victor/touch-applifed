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
#  * Description: Testcases for the periodic re-prestart functionality 
#   
#  * Objectives: test the functionality of applifed from
#    the periodic re-prestart point of view
#

require 'tdriver'
require 'date'
require 'test/unit'
include TDriverVerify


class TC_PERIODICREPRESTART < Test::Unit::TestCase

    TIMES_TO_BE_RESTARTED = 5
    
    #Get the uname to find where are the tests running
    $path = string = `echo $PATH `

    # method called before any test case
    def setup
        #kill applications that are already running 
	app = 'fali_hello'
	    if system("pgrep #{app}") == true
	        system("kill -9 `pgrep #{app}`")
	    end

        #make sure that log file does not exist before the test run
	if FileTest.exists?("/tmp/periodic-re-prestart.log")
	  system "rm /tmp/periodic-re-prestart.log"
	end

        if $path.include?("scratchbox")
            puts "Inside SB, Do Nothing to unlock"
            @daemon_running = system('pkill applifed')
            #start applifed in test mode and prestart the listed applications
	    system("source /tmp/session_bus_address.user;" +
                   "DISPLAY=:1 applifed --test --log /tmp/periodic-re-prestart.log" +
                   "< /usr/share/applifed-M-testscripts/periodic_re_prestart/periodic_re_prestart.drive &")
        else
	    system "mcetool --set-tklock-mode=unlocked"
            @daemon_running = system('initctl stop xsession/applifed')
            #start applifed in test mode and prestart the listed applications
	    system("source /tmp/session_bus_address.user;" +
                   "DISPLAY=:0 applifed --test --log /tmp/periodic-re-prestart.log" +
                   "< /usr/share/applifed-M-testscripts/periodic_re_prestart/periodic_re_prestart.drive &")
        end
        @sut = TDriver.sut(:Id=>ARGV[0] || 'sut_qt_maemo')   
	sleep 5 
    end
    
    # method called after any test case for cleanup purposes
    def teardown
        if @daemon_running
            if $path.include?("scratchbox") 
                system('applifed &')
            else
                system('initctl start xsession/applifed')
            end
        end
    end
    
    #To test that the application is prestarted by applifed 

    def test_periodic_re_prestart
        $pid = string = `pgrep fali_hello`
        for i in 1..TIMES_TO_BE_RESTARTED 
            string = `dbus-send --dest=com.nokia.fali_hello --type="method_call" /org/maemo/m com.nokia.MApplicationIf.launch`
            sleep(2)
            @app = @sut.application(:name => 'fali_hello')
            newpid = string = `pgrep fali_hello`
            verify_true(30,"Application is not prestarted"){$pid == newpid}
            @app.MButton( :name => 'CloseButton').tap
        end
        verify_equal(true,30,"Screen blanking did not happen"){
            system "grep \"Application 'com.nokia.fali_hello' scheduled for re-prestart\" /tmp/periodic-re-prestart.log"}
        sleep 6
        newpid = string = `pgrep fali_hello`
        puts newpid
        verify_true(30,"Application is not prestarted"){$pid != newpid}
    end
end
