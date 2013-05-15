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
#  * Description: Testcases for the prestart functionality 
#   
#  * Objectives: test the functionality of applifed from
#    the prestart point of view
#

require 'tdriver'
require 'date'
require 'test/unit'
include TDriverVerify


class TC_PrestartTDriverTests < Test::Unit::TestCase

    TIMES_TO_BE_RESTARTED = 3 
    MULTI_APP_PRESTART_TIME = 20
    
    # method called before any test case
    def setup
        #make sure that log file does not exist before the test run
	if FileTest.exists?("/tmp/applifed-test.log")
	  system "rm /tmp/applifed-test.log"
	end
        #kill applications that are already running 
	apps = ['fali_hello',
		'fali_toc']
	for app in apps
	  if system("pgrep #{app}") == true
	    system("kill -9 `pgrep #{app}`")
	    sleep 1
	    system("kill -9 `pgrep #{app}`")
	  end
	end

	system "mcetool --set-tklock-mode=unlocked"
        system "pkill -STOP applifed"
	cmd = '"DISPLAY=:0 applifed --test --log /tmp/applifed-test.log < /usr/share/applifed-M-testscripts/functional.drive &"'
        system "su - meego -c #{cmd}"
        @sut = TDriver.sut(:Id=>ARGV[0] || 'sut_qt_maemo')   

	sleep 5 
    end
    
    # method called after any test case for cleanup purposes
    def teardown
        system "pkill -CONT applifed"
    end
    
    #To test that the application is prestarted by applifed 

    def test_prestart
        $pid = string = `pgrep fali_hello`
        for i in 1..TIMES_TO_BE_RESTARTED 
            sleep(15)
            cmd = '"dbus-send --dest=com.nokia.fali_hello --type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch"'
            system "su - meego -c #{cmd}"
            sleep(8)
            @app = @sut.application(:name => 'fali_hello')
            newpid = string = `pgrep fali_hello`
            verify_true(30,"Application is not prestarted"){$pid == newpid}
            @app.MStylableWidget.MButton.tap
        end
    end
    
    #To test that the application is re-prestarted by applifed 
    def test_re_prestart
        verify_equal(true,30,"Applifed not running"){system "pgrep applifed"}
        
        pid = string = `pgrep fali_toc`       
        
        sleep(25)
        cmd ='"dbus-send --dest=com.nokia.fali_toc --type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch"'
        system "su - meego -c #{cmd}"
        sleep(8)
        @app = @sut.application(:name => 'fali_toc')
        
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"Application 'com.nokia.fali_toc' released from prestarted state\" /tmp/applifed-test.log"}
        
        close_button = @app.MStylableWidget.MButton
        close_button.tap
        sleep(15)
        newpid = string = `pgrep fali_toc`
        verify_true(30,"Re-prestarting Not done"){pid != newpid}
        
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"Re-prestarting 'com.nokia.fali_toc\" /tmp/applifed-test.log"}
    end
 
    #To test that the application is re-prestarted by applifed after they are killed
    def test_kill_re_prestart
        verify_equal(true,30,"Applifed not running"){system "pgrep applifed"}
        
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"com.nokia.fali_toc' got registered\" /tmp/applifed-test.log"}

        verify_equal(true,30,"Application not prestarted"){
            system "grep \"com.nokia.fali_hello' got registered\" /tmp/applifed-test.log"}

        sleep(10)

        cmd = '"dbus-send --dest=com.nokia.fali_toc --type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch"'
        system "su - meego -c #{cmd}"
        sleep (6)
        cmd1 = '"dbus-send --dest=com.nokia.fali_hello --type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch"'
        system "su - meego -c #{cmd1}"
        
        sleep(8)
        
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"Application 'com.nokia.fali_toc' released from prestarted state\" /tmp/applifed-test.log"}

        verify_equal(true,30,"Application not prestarted"){
            system "grep \"Application 'com.nokia.fali_hello' released from prestarted state\" /tmp/applifed-test.log"}

        pid_toc = string = `pgrep fali_toc`       
        pid_ah = string = `pgrep fali_hello`       

        verify_equal(true,30,"fali_toc not killed"){system "pkill fali_toc"}
        verify_equal(true,30,"fali_hello not killed"){system "pkill fali_hello"}
        sleep(5)
        
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"'com.nokia.fali_toc' got unregistered..\" /tmp/applifed-test.log"}

        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"Re-prestarting 'com.nokia.fali_toc\" /tmp/applifed-test.log"}

        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"'com.nokia.fali_hello' got unregistered..\" /tmp/applifed-test.log"}

        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"Re-prestarting 'com.nokia.fali_hello'\" /tmp/applifed-test.log"}
    end
    

end
