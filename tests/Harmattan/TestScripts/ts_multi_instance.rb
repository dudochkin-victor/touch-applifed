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
#    the prestart point of view for multiwindows
#

require 'tdriver'
require 'date'
require 'test/unit'
include TDriverVerify
ADDITIONAL_PATH=File.join("/usr/share/applifed-testscripts")
$LOAD_PATH << ADDITIONAL_PATH

require 'fautils'

class TC_MWTESTS < Test::Unit::TestCase
    
    MULTI_APP_PRESTART_TIME = 20 
    $path = string = `echo $PATH `

    # method called before any test case
    def setup
        @tc = Fali_utils.new
        @tc.print_debug("Enter setup")
        @tc.get_pids('applifed')
    end

    # method called after any test case for cleanup purposes
    def teardown
        @tc.run_system_command(:cmd => "pkill #{@appname}", \
                               :failmsg => "Failed to kill #{@appname}", \
                               :docheck => true, :sleep => 0.2, \
                               :logmsg => "killing #{@appname}")
        @tc.get_pids('applifed')
        @tc.print_debug("exit from teardown")
    end

    
    def test_multi_instance_prestart
        #Test to prestart n number of applications at the same time
        @appname = 'fali_multiapp'
        for i in 1..MULTI_APP_PRESTART_TIME
            if $path.include?("scratchbox")
                @tc.run_system_command(:cmd => "#{@appname} #{i} -prestart -software&", \
                               :failmsg => "Failed to prestart #{@appname} #{i} ", \
                               :docheck => true, :sleep => 0.2, \
                               :logmsg => "prestarting #{@appname} #{i} ")

            else
                @tc.run_system_command(:cmd => "#{@appname} #{i} -prestart &", \
                               :failmsg => "Failed to prestart #{@appname} #{i} ", \
                               :docheck => true, :sleep => 0.2, \
                               :logmsg => "prestarting #{@appname} #{i} ")

            end
            sleep 2
        end
        pids = @tc.get_pids("#{@appname}")
        assert_equal(MULTI_APP_PRESTART_TIME,pids.length , "All applications were not prestarted")

    end

end
