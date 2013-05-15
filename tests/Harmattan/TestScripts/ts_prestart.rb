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

ADDITIONAL_PATH=File.join("/usr/share/applifed-testscripts")
$LOAD_PATH << ADDITIONAL_PATH
require 'fautils'

class TC_PrestartTDriverTests < Test::Unit::TestCase

    #Get the uname to find where are the tests running
    $path = string = `echo $PATH `

    # method called before any test case
    def setup
        #make sure that log file does not exist before the test run
	@tc = Fali_utils.new
        @tc.get_pids('applifed')
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

        if $path.include?("scratchbox")
            puts "Inside SB, Do Nothing to unlock"
            @daemon_running = system('pkill applifed')
            mydisplay = 1
        else
	    system "mcetool --set-tklock-mode=unlocked"
            @daemon_running = system('initctl stop xsession/applifed')
            mydisplay = 0
        end
	@tc.run_system_command(:cmd => "source /tmp/session_bus_address.user;" + \
			       "DISPLAY=:#{mydisplay}; applifed --test --log /tmp/applifed-test.log" + \
			       " < /usr/share/applifed-testscripts/functional.drive &", \
			       :failmsg => "unable to start applifed in test mode", \
			       :docheck => true, :sleep => 5, \
			       :logmsg => "start applifed in test mode")
        @sut = TDriver.sut(:Id=>ARGV[0] || 'sut_qt_maemo')   
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
        @tc.get_pids('applifed')
    end
    
    #To test that the application is re-prestarted by applifed after they are killed
    def test_kill_re_prestart
        #verify_equal(true,30,"Applifed not running"){system "pgrep applifed"}
        @tc.run_system_command(:cmd => "pgrep applifed", \
			       :failmsg => "applifed not running", \
			       :docheck => true, :sleep => 1, \
			       :logmsg => "checking for applifed")
        @tc.print_debug("checking for fali_toc registered to applifed") 
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"com.nokia.fali_toc' got registered\" /tmp/applifed-test.log"}

        @tc.print_debug("checking for fali_hello registered to applifed") 
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"com.nokia.fali_hello' got registered\" /tmp/applifed-test.log"}

        @tc.run_system_command(:cmd => "dbus-send --dest=com.nokia.fali_toc " + \
                               "--type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch", \
			       :failmsg => "unable to start fali_toc", \
			       :docheck => true, :sleep => 2, \
			       :logmsg => "starting fali_toc")

        @tc.run_system_command(:cmd => "dbus-send --dest=com.nokia.fali_hello " + \
                               "--type=\"method_call\" /org/maemo/m com.nokia.MApplicationIf.launch", \
			       :failmsg => "unable to start fali_hello", \
			       :docheck => true, :sleep => 2, \
			       :logmsg => "starting fali_hello")
        
        @tc.print_debug("checking for fali_toc release from prestarted state") 
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"Application 'com.nokia.fali_toc' released from prestarted state\" /tmp/applifed-test.log"}

        @tc.print_debug("checking for fali_hello release from prestarted state") 
        verify_equal(true,30,"Application not prestarted"){
            system "grep \"Application 'com.nokia.fali_hello' released from prestarted state\" /tmp/applifed-test.log"}

        pid_toc = @tc.get_pids("fali_toc")
        pid_ah = @tc.get_pids("fali_hello")

        @tc.print_debug("kill fali_toc") 
        verify_equal(true,30,"fali_toc not killed"){system "pkill fali_toc"}

        @tc.print_debug("kill fali_hello") 
        verify_equal(true,30,"fali_hello not killed"){system "pkill fali_hello"}
        sleep(4)
        
        @tc.print_debug("Checking that fali_toc got unregistered") 
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"'com.nokia.fali_toc' got unregistered..\" /tmp/applifed-test.log"}

        @tc.print_debug("Checking that fali_toc was re-prestarted") 
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"Re-prestarting 'com.nokia.fali_toc\" /tmp/applifed-test.log"}

        @tc.print_debug("Checking that fali_hello got unregistered") 
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"'com.nokia.fali_hello' got unregistered..\" /tmp/applifed-test.log"}

        @tc.print_debug("Checking that fali_hello was re-prestarted") 
        verify_equal(true,30,"Application not Re-prestarted"){
            system "grep \"Re-prestarting 'com.nokia.fali_hello'\" /tmp/applifed-test.log"}

        pid_toc_n = @tc.get_pids("fali_toc")
        pid_ah_n = @tc.get_pids("fali_hello")

        @tc.print_debug("Checking that fali_toc was registered with new pid") 
        verify_equal(true, 30, "fali_toc was not re-prestarted"){
            pid_toc != pid_toc_n}

        @tc.print_debug("Checking that fali_hello was registered with new pid") 
        verify_equal(true, 30, "fali_toc was not re-prestarted"){
        pid_ah != pid_ah_n}
    end
end
