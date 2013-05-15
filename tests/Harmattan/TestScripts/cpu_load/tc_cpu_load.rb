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
#  * Objectives:  To test that applications are prestarted based 
#    on the cpu load.
#

require 'tdriver'
require 'test/unit'
include TDriverVerify

ADDITIONAL_PATH=File.join("/usr/share/applifed-testscripts")
$LOAD_PATH << ADDITIONAL_PATH
require 'fautils'

class TC_CpuLoadTests < Test::Unit::TestCase

    $path = string = `echo $PATH `
  # method called before any test case
  def setup
    @tc = Fali_utils.new
    @tc.get_pids('applifed')
    if $path.include?("scratchbox")
        puts "Inside SB, Do Nothing to unlock"
        @mydisplay = 1
    else
        @mydisplay = 0
        system "mcetool --set-tklock-mode=unlocked"
    end
    apps = ['fali_hello',
            'fali_mw2',
            'fali_toc']

    for app in apps
      if system("pgrep #{app}") == true
        @tc.print_debug("Killing #{app}")
        system("kill -9 `pgrep #{app}`")
      end
    end
  end
  
  # method called after any test case for cleanup purposes
  def  teardown
    @tc.get_pids('applifed')
  end

  # Test that a applications are prestarted when the cpu
  # load is lower than threshold value
  def test_cpu_load_low
    if FileTest.exists?("/tmp/cpuload_low.log")
      system "rm /tmp/cpuload_low.log"
    end

    @tc.run_system_command(:cmd => "source /tmp/session_bus_address.user;" +
	                   "DISPLAY=:#{@mydisplay}; applifed --test --log /tmp/cpuload_low.log" +
	                   "< /usr/share/applifed-testscripts/cpu_load/cpu_load_low.drive &", \
			   :failmsg => "unable to start applifed in test mode with cpu load low", \
			   :docheck => true, :sleep => 6, \
			   :logmsg => "start applifed with low cpu load")
    @tc.print_debug("Check that fali_toc prestarted with Priority 1")
    verify_equal(true,30,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_toc', priority=1\" /tmp/cpuload_low.log"}

    @tc.print_debug("Check that fali_hello got registered")
    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_hello' got registered\" /tmp/cpuload_low.log"}

    @tc.print_debug("Check that fali_mw2 prestarted with priority 2")
    verify_equal(true,2,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_mw2', priority=2\" /tmp/cpuload_low.log"}

    @tc.print_debug("Check that fali_toc got registered")
    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_toc' got registered\" /tmp/cpuload_low.log"}

    @tc.print_debug("Check that fali_hello prestarted with priority 3")
    verify_equal(true,2,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_hello', priority=3\" /tmp/cpuload_low.log"}

    @tc.print_debug("Check that fali_mw2 got registered")
    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_mw2' got registered\" /tmp/cpuload_low.log"}
  end

  # Test that applications are not prestarted when the cpu
  # load is higher than threshold value
  def test_cpu_load_high
    if FileTest.exists?("/tmp/cpuload_high.log")
      system "rm /tmp/cpuload_high.log"
    end

    @tc.run_system_command(:cmd => "source /tmp/session_bus_address.user;" +
	                   "DISPLAY=:#{@mydisplay}; applifed --test --log /tmp/cpuload_high.log" +
	                   "< /usr/share/applifed-testscripts/cpu_load/cpu_load_high.drive &", \
			   :failmsg => "unable to start applifed in test mode with cpu load high", \
			   :docheck => true, :sleep => 6, \
			   :logmsg => "start applifed with high cpu load")
    sleep 6

    @tc.print_debug("Check that fali_hello prestarted with priority 3")
    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_hello', priority=3\" /tmp/cpuload_high.log"}

    @tc.print_debug("Check that fali_toc prestarted with Priority 1")
    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_toc', priority=1\" /tmp/cpuload_high.log"}

    @tc.print_debug("Check that fali_mw2 prestarted with priority 2")
    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_mw2', priority=2\" /tmp/cpuload_high.log"}
  end        

end
