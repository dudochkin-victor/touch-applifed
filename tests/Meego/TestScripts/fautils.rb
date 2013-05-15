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

module FaUtils
    # Returns a list of pids for an application.
    # Returns nil if no application was found to be running.
    def get_pids(app)
        pids = `pgrep #{app}`.split(/\s/).collect { |x| x.strip() }.delete_if { |x| x.empty? }
        pids = nil if pids.empty?
        return pids
    end

    # Returns the pid of an application or nil if it wasn't running.
    def get_pid(app)
        pids = get_pids(app)

        if not pids.nil? and pids.length >= 1
            return pids[0]
        end
           
        return nil
    end

    # Kill an application. Returns true if killed, false otherwise.
    def kill(app)
        system("pkill -9 #{app}")
    end

    # Returns true is at least one instance of application is running
    def running?(app)
        not get_pid(app).empty?
    end
end
