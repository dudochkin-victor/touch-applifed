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
require "tdriver"
include TDriverVerify
include Test::Unit::Assertions

class Fali_utils
    def failure(msg)
        xx = "#########\nFailure:\n#{msg}\n#########"
        return xx
    end
    def print_debug(msg)
        message = "[INFO]  #{msg}\n"
        puts message
    end

    def get_pids(app)
        pids = `pgrep #{app}`.split(/\s/).collect { |x| x.strip() }.delete_if { |x| x.empty? }
        print_debug("The pids of #{app} is : #{pids.join(' ')}")
        pids = nil if pids.empty?
        return pids
    end

    # Returns the pid of an application or nil if it wasn't running.
    def get_pid(app)
        print_debug("get the pids of #{app}")
        pids = get_pids(app)

        if not pids.nil? and pids.length >= 1
            print_debug("The pids of #{app} is : #{pids.join(' ')}")
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

    def run_system_command(ahash = {})
        if ahash[:logmsg]
            print_debug(ahash[:logmsg])
        end
        myretval = system(ahash[:cmd])
        if ahash[:sleep]
            print_debug("sleeping for #{ahash[:sleep]} seconds")
            sleep(ahash[:sleep])
            print_debug("wake up now")
        end
        if ahash[:docheck] 
            assert_equal(true, myretval, ahash[:failmsg])
        end
        if ahash[:ret_cmd_val]
            return myretval
        end
    end

    def kill_unwanted_apps
        @sut = TDriver.sut(:Id => 'sut_qt_maemo')
        begin 
            @app = @sut.application
            @exePath = @app.attribute('exepath')
            print_debug("The exepath of on-top application is :#{@exePath}")
            if @exePath == "/usr/bin/applauncherd.bin"
                @appName = @app.name 
                print_debug("The application on-top is :#{@exePath}")
            else
                @appName = File.basename(@exePath)
                print_debug("The application on-top is :#{@appName}")
            end
         end
           
        while @appName != "meegotouchhome"
            system("pkill #{@appName}")
            @app = @sut.application
            @exePath = @app.attribute('exepath')
            print_debug("The exepath of on-top application is :#{@exePath}")
            if @exePath == "/usr/bin/applauncherd.bin"
                @appName = @app.name 
                print_debug("The application on-top is :#{@exePath}")
            else
                @appName = File.basename(@exePath)
                print_debug("The application on-top is :#{@appName}")
            end
        end
        print_debug("Now meegotouchhome is on-top")

    end
    def open_Apps_from_grid(appName)
        #Open the Application from the application grid
        print_debug("searching for application #{appName} in appGrid...")
        @meegoHome = @sut.application(:name => 'meegotouchhome')
	if @meegoHome.test_object_exists?("LauncherButton", :text => appName)
	  icon = @meegoHome.LauncherButton(:name => "LauncherButton", :text => appName)

	  grid = nil
	  if @meegoHome.test_object_exists?("Launcher" ) 
	    grid = @meegoHome.Launcher
	  elsif @meegoHome.test_object_exists?("SwipeLauncher" ) 
	    grid = @meegoHome.SwipeLauncher
	  else
	    raise "Launcher not found!"
	    exit 1
	  end

	  while icon.attribute('visibleOnScreen') == 'false' || icon.attribute('y').to_i > 400
	    grid.MPannableViewport( :name => 'SwipePage' ).MWidget( :name => 'glass' ).gesture(:Up, 1, 300)
	    sleep(0.2)
	    icon.refresh
	  end
            print_debug("Application #{appName} found : now tapping on it")
            @meegoHome.LauncherButton(:name => "LauncherButton", :text => appName).tap
       else
            #icon does not
            #raise error and exit
            raise "Application not found in Application grid"
            exit 1
       end
    end
end
