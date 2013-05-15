#!/usr/bin/ruby
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
#  * Description: Prestart preventing test for applifed 
#   
#  * Objectives: test the prestart preventing feature for applifed
#    

require 'tdriver'
require 'date'
require 'test/unit'
require 'test/unit/assertions'
include Test::Unit::Assertions
include TDriverVerify

ADDITIONAL_PATH=File.join("/usr/share/applifed-testscripts")
$LOAD_PATH << ADDITIONAL_PATH
require 'fautils'

class TC_Prestart_Prevent < Test::Unit::TestCase

    $path = string = `echo $PATH `
    # method called before any test case
    def setup
        @tc.get_pids('applifed')
        @appName = "fali_close"
	@tc = Fali_utils.new
        if $path.include?("scratchbox")
            puts "Inside SB, Do Nothing to unlock"
	  if system("pgrep #{@appName}") == true
	    system("kill -9 `pgrep #{@appName}`")
          end
        else
	    system("mcetool --set-tklock-mode=unlocked")
	    system("initctl restart xsession/mthome")
            sleep (5)
	  if system("pgrep #{@appName}") == true
	    system("kill -9 `pgrep #{@appName}`")
          end
        end
        @sut = TDriver.sut(:Id=> 'sut_qt_maemo')
    end

    # method called after any test case for cleanup purposes
    def teardown
        @tc.get_pids('applifed')
        puts "exit from teardown"
    end

    def read_file
       #Reading the log file to get the time
       file_name="/tmp/applifed_perftest.log"
       last_line = `tail -n 2 #{file_name}`
       @end_time = last_line.split(" ")[0] 
    end

    def test_grid_prestart_prevent
        @tc.kill_unwanted_apps
        @tc.open_Apps_from_grid(@appName)
        @app = @sut.application(:name => @appName)
        pid = @tc.get_pids(@appName)
        verify_true(2, "The Application was not launched"){
            @app.test_object_exists?("MButton",{:text => 'close'})}
        @app.MButton( :text => 'close' ).tap
        sleep(2)
        newpid = @tc.get_pids(@appName)
        assert(newpid == nil, "#{@appName} was not killed")
    end

    def test_cmdlile_prestart_prevent
        @tc.kill_unwanted_apps
        system("su - user -c 'fali_close -prestart'&")        
        sleep(2)
        system("#{@appName}&")        
        @tc.print_debug("launching #{@appName}")
        @app = @sut.application(:name => @appName)
        pid = @tc.get_pids(@appName)
        verify_true(2, "The Application was not launched"){
            @app.test_object_exists?("MButton",{:text => 'close'})}
        @app.MButton( :text => 'close' ).tap
        sleep(2)
        newpid = @tc.get_pids(@appName)
        assert(newpid == nil, "#{@appName} was not killed")
    end
end
