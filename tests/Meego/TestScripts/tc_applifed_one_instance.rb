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

class TC_OnlyOneApplifed < Test::Unit::TestCase
    # Check that only one instance of applifed is running
    def get_pids(app)
        pids = `pgrep #{app}`.split(/\s/).collect { |x| x.strip() }.delete_if { |x| x.empty? }
        pids = nil if pids.empty?
        return pids
    end

    def test_only_one_applifed
        pids = get_pids('applifed')

        if not pids.nil?
            puts "pids: #{pids.join(' ')}"
        else
            verify_true(0, "Applifed is not running") { false }
        end
        
        verify_true(0, "Multiple instances of applifed running") {
            pids.length == 1
        }
    end

    def test_lock()
        verify_true(5, "applifed not running") {
            system("pgrep applifed")
        }

        verify_false(5, "only one instance should be allowed to run") {
            system("applifed --fork")
        }
    end
end

