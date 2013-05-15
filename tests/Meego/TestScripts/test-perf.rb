#!/usr/bin/ruby
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# Contact: Nokia Corporation (directui@nokia.com)
#
# This file is part of applauncherd.
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
#  * Description: Performance Test for applauncherd 
#   
#  * Objectives: test the startup time for applications
#    

require 'tdriver'
require 'date'
require 'test/unit'
require 'test/unit/assertions'
include Test::Unit::Assertions
include TDriverVerify


class TC_PerformanceTests < Test::Unit::TestCase
    COUNT = 5 
    @start_time = 0
    @end_time = 0
    
    $path = string = `echo $PATH `

    # method called before any test case
    def setup
        if $path.include?("scratchbox")
            puts "Inside SB, Do Nothing to unlock"
            @daemon_running = system('pkill applifed')
	    if system("pgrep fali_hello") == true
	        system("kill -9 `pgrep fali_hello`")
            end
            sleep (1)
            system "/usr/bin/fali_hello -prestart &"
        else
	    system "mcetool --set-tklock-mode=unlocked"
            # restart mthome so that qttasserver notices it
            verify { 
                system("/sbin/initctl restart xsession/mthome")
            }
            @daemon_running = system('initctl stop xsession/applifed')
	    if system("pgrep fali_hello") == true
	        system("kill -9 `pgrep fali_hello`")
            end
            sleep (1)
            system "/usr/bin/fali_hello -prestart &"
            sleep (3)
        end
        @sut = TDriver.sut(:Id=> 'sut_qt_maemo')
    end

    # method called after any test case for cleanup purposes
    def teardown
        puts "exit from teardown"
        if @daemon_running
            if $path.include?("scratchbox") 
                system('applifed &')
            else
                system('initctl start xsession/applifed')
            end
        end
    end

    def open_Apps(appName)
        #Remove the Log file if it exists
        if FileTest.exists?("/tmp/applifed_perftest.log")
          system "rm /tmp/applifed_perftest.log"
        end

        count = 0

        #Open the Application from the application grid
        @meegoHome = @sut.application(:name => 'meegotouchhome')
        @meegoHome.MButton(:name => "ToggleLauncherButton").tap
        sleep(2)
        if @meegoHome.test_object_exists?("LauncherButton", :text => appName)
            icon = @meegoHome.LauncherButton(:name => "LauncherButton", :text => appName)
            totalPages = @meegoHome.children(:type => 'LauncherPage').length
            while icon.attribute('visibleOnScreen') == 'false' and count < totalPages
                @meegoHome.PagedViewport(:name => 'LauncherPagedViewport').MWidget(:name => 'glass').gesture(:Left, 1, 800)
                sleep(0.2)
                count = count +1 
                icon.refresh
            end
            @start_time = Time.now
            @meegoHome.LauncherButton(:name => "LauncherButton", :text => appName).tap
            sleep (2)
            @app = @sut.application(:name => appName)
            sleep (2)
            @app.MEscapeButtonPanel.MButton( :name => 'CloseButton' ).tap
       else
            #icon does not
            #raise error and exit
            raise "Application not found in Application grid"
            exit 1
       end
    end

    def read_file
       #Reading the log file to get the time
       file_name="/tmp/applifed_perftest.log"
       last_line = `tail -n 2 #{file_name}`
       @end_time = last_line.split(" ")[0] 
    end

    def measure_time
       #Measuring the Startup Time for applications
       start_t = "%10.6f" % @start_time.to_f
       app_t = Float(@end_time) - Float(start_t)
       return app_t
    end

    def test_performance
      wP = []
      woP = []
      wPsum = 0.0
      woPsum = 0.0
     
      #Run Application with invoker
      for i in 1..COUNT
          open_Apps("fali_hello")
          print "Now Launching fali_hello %d times\n" %i
          sleep (5)
          read_file
          wP.push(measure_time)
      end

      #Run Application without invoker
      for i in 1..COUNT
          open_Apps("fali_hello1")
          print "Now Launching fali_hello1 %d times\n" %i
          sleep (5)
          read_file
          woP.push(measure_time)
      end

      print "With Prestart \t\t Without Prestart\n"

      #Printing the data
      for i in 0..COUNT-1
          print "%.2f \t\t\t %.2f\n" %[wP[i],woP[i]]
          wPsum = wPsum + wP[i]
          woPsum = woPsum + woP[i]
      end
      print "\nAverage Values \n"
      print "%.2f \t\t\t %.2f\n\n" %[wPsum/COUNT, woPsum/COUNT]
      assert(wPsum/COUNT <= 0.75, "Application takes longer time to launch")
       
    end
end
