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


class TC_CpuLoadTests < Test::Unit::TestCase

    $path = string = `echo $PATH `
  # method called before any test case
  def setup
    if $path.include?("scratchbox")
        puts "Inside SB, Do Nothing to unlock"
    else
        system "mcetool --set-tklock-mode=unlocked"
    end
    apps = ['fali_hello',
            'fali_mw2',
            'fali_toc']

    for app in apps
      if system("pgrep #{app}") == true
        system("kill -9 `pgrep #{app}`")
        sleep 1
        system("kill -9 `pgrep #{app}`")
      end
    end
  end
  
  # method called after any test case for cleanup purposes
  def teardown
  end

  # Test that a applications are prestarted when the cpu
  # load is lower than threshold value
  def test_cpu_load_low
    if FileTest.exists?("/tmp/cpuload_low.log")
      system "rm /tmp/cpuload_low.log"
    end

    if $path.include?("scratchbox")
        system("source /tmp/session_bus_address.user;" +
               "DISPLAY=:1 applifed --test --log /tmp/cpuload_low.log" +
               "< /usr/share/applifed-M-testscripts/cpu_load/cpu_load_low.drive &")
    else
        system("source /tmp/session_bus_address.user;" +
               "DISPLAY=:0 applifed --test --log /tmp/cpuload_low.log" +
               "< /usr/share/applifed-M-testscripts/cpu_load/cpu_load_low.drive &")
    end
    sleep 6

    verify_equal(true,30,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_toc', priority=1\" /tmp/cpuload_low.log"}

    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_hello' got registered\" /tmp/cpuload_low.log"}

    verify_equal(true,2,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_mw2', priority=2\" /tmp/cpuload_low.log"}

    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_toc' got registered\" /tmp/cpuload_low.log"}

    verify_equal(true,2,"Application not prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_hello', priority=3\" /tmp/cpuload_low.log"}

    verify_equal(true,2,"Application not prestarted"){
      system "grep \"'com.nokia.fali_mw2' got registered\" /tmp/cpuload_low.log"}
  end

  # Test that applications are not prestarted when the cpu
  # load is higher than threshold value
  def test_cpu_load_high
    if FileTest.exists?("/tmp/cpuload_high.log")
      system "rm /tmp/cpuload_high.log"
    end

    if $path.include?("scratchbox")
        system("source /tmp/session_bus_address.user;" +
               "DISPLAY=:1 applifed --test --log /tmp/cpuload_high.log" +
               "< /usr/share/applifed-M-testscripts/cpu_load/cpu_load_high.drive &")
    else
        system("source /tmp/session_bus_address.user;" +
               "DISPLAY=:0 applifed --test --log /tmp/cpuload_high.log" +
               "< /usr/share/applifed-M-testscripts/cpu_load/cpu_load_high.drive &")
    end
    sleep 6

    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_hello', priority=3\" /tmp/cpuload_high.log"}

    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_toc', priority=1\" /tmp/cpuload_high.log"}

    verify_equal(false,2,"Application is prestarted"){
      system "grep \"Prestarting 'com.nokia.fali_mw2', priority=2\" /tmp/cpuload_high.log"}
  end        

end
