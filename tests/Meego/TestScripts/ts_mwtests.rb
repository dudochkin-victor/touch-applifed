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

class TC_MWTESTS < Test::Unit::TestCase
    
    TIMES_TO_BE_RESTARTED = 5 
    $path = string = `echo $PATH `

    # method called before any test case
    def setup
        if $path.include?("scratchbox")
            puts "Inside SB, Do Nothing to unlock"
        else
	    system "mcetool --set-tklock-mode=unlocked"
            # restart meegotouchhome so that qttasserver notices it
            verify {
                system("/sbin/initctl restart xsession/mthome")
            }
        end
        @sut = TDriver.sut(:Id=>ARGV[0] || 'sut_qt_maemo')
    end

    # method called after any test case for cleanup purposes
    def teardown
        kill_application()
        @appname = nil
        puts "exit from teardown"
    end

    def test_mw3_prestart
        #Test for a multiwindow application inherited from MApplication and override windows CloseEvents tested
        @appname = 'fali_mw3'

	start_exec()
	run_dbus_command_multiwindow('fali_mw3', '1')
	run_dbus_command_multiwindow('fali_mw3', '2')
	run_dbus_command_multiwindow('fali_mw3', '3')

	pid = string = `pgrep #{@appname}`
	@app = @sut.application(:name => @appname) 
	@app.MyWindow( :name => 'Window 3' ).MainPage.MPannableViewport.MLabel(:text => 'Modify content').tap
	@app.MyWindow( :name => 'Window 3' ).MButton( :name => 'CloseButton' ).tap
	@app.MyWindow( :name => 'Window 3' ).MButton( :name => 'MDialogButtonYes' ).tap

        system "mcetool --set-tklock-mode=unlocked"
	@app.MApplicationWindow( :name => 'Window 2' ).MLabel(:text => 'Modify content').tap
	@app.MApplicationWindow( :name => 'Window 2' ).MButton( :name => 'CloseButton' ).tap
	@app.MApplicationWindow( :name => 'Window 1' ).MLabel(:text => 'Modify content').tap
	@app.MApplicationWindow( :name => 'Window 1' ).MButton( :name => 'CloseButton' ).tap
       
        #verifies that the multiwindow application is lazy shutdown and has the same pid after it is closed 
	newid = string = `pgrep #{@appname}`	
	verify_true(30,"The application is not prestarted"){pid == newid}

        run_dbus_command_multiwindow('fali_mw3', '3')
	verify(){@app.MyWindow( :name => 'Window 3' ).MLabel(:text => 'Fresh Window 3')}

    end	

    def test_mw3_switcher
	#Test that all window of a multiwindow application exists in the switcher
        @appname = 'fali_mw3'

        start_exec()
        run_dbus_command_multiwindow('fali_mw3', '1')
        run_dbus_command_multiwindow('fali_mw3', '2')
        run_dbus_command_multiwindow('fali_mw3', '3')

	pid = string = `pgrep #{@appname}`
	@app = @sut.application(:name => @appname) 
	@app.MyWindow( :name => 'Window 3' ).MHomeButtonPanel( :name => 'MHomeButtonPanel').MButton( :name => 'HomeButton').tap
	@app_home = @sut.application(:name => 'meegotouchhome')
        
        #verifies that all the three windows exist in the switcher
	verify(){@app_home.SwitcherButton(:text => 'Window 1')}
	verify(){@app_home.SwitcherButton(:text => 'Window 2')}
	verify(){@app_home.SwitcherButton(:text => 'Window 3')}

    end

    def test_mw2_close
	#Test for multiwindow re-prestart
        @appname = 'fali_mw2'
	if system("pgrep #{@appname}") == true
	    system("kill -9 `pgrep #{@appname}`")
        end
        if $path.include?("scratchbox")
	    system("source /tmp/session_bus_address.user;" +
                   "DISPLAY=:1 applifed --test --log /tmp/applifed-test.log" +
                   " < /usr/share/applifed-M-testscripts/functional.drive &")
        else
	    system("source /tmp/session_bus_address.user;" +
                   "DISPLAY=:0 applifed --test --log /tmp/applifed-test.log" +
                   " < /usr/share/applifed-M-testscripts/functional.drive &")
        end
        sleep 10 
        run_dbus_command_multiwindow('fali_mw2', '1')
        run_dbus_command_multiwindow('fali_mw2', '2')
        run_dbus_command_multiwindow('fali_mw2', '3')

	pid = string = `pgrep #{@appname}`
	@app = @sut.application(:name => @appname) 
	@app.MApplicationWindow( :name => 'Window 3' ).MButton( :name => 'HomeButton' ).tap
	@app_home = @sut.application(:name => 'meegotouchhome')

	verify(){@app_home.SwitcherButton(:text => 'Window 1')}                                                                     
	verify(){@app_home.SwitcherButton(:text => 'Window 2')}                                                                     
	verify(){@app_home.SwitcherButton(:text => 'Window 3')}                                                                    
        system "mcetool --set-tklock-mode=unlocked"
	@app_home.SwitcherButton(:text => 'Window 1').tap
	@app.MApplicationWindow( :name => 'Window 1' ).MButton( :name => 'CloseButton' ).tap
	@app_home.SwitcherButton(:text => 'Window 2').tap
	@app.MApplicationWindow( :name => 'Window 2' ).MButton( :name => 'CloseButton' ).tap
	@app_home.SwitcherButton(:text => 'Window 3').tap 
	@app.MApplicationWindow( :name => 'Window 3' ).MButton( :name => 'CloseButton' ).tap
	sleep(6)

        #verifies that multiwindow application is a terminate and close application and is re-prestarted with a new pid
	newid = string = `pgrep #{@appname}`
	verify_true(30,"The application is not prestarted"){pid != newid}
    end                                                             


    def test_mw1_close
	#Test for multiwindow prestart and lazy shutdown
        @appname = 'fali_mw1'

        start_exec()
        #runs the test in a loop 
        for i in 1..TIMES_TO_BE_RESTARTED
            if $path.include?("scratchbox")
                puts "Inside SB, Do Nothing to unlock"
            else
                system "mcetool --set-tklock-mode=unlocked"
            end
	    run_dbus_command_multiwindow('fali_mw1', '1')
	    run_dbus_command_multiwindow('fali_mw1', '2')
	    run_dbus_command_multiwindow('fali_mw1', '3')

	    pid = string = `pgrep #{@appname}`
	    @app = @sut.application(:name => @appname) 
	    @app.MApplicationWindow( :name => 'Window 3' ).MButton( :name => 'HomeButton' ).tap
	    @app_home = @sut.application(:name => 'meegotouchhome')

	    verify(){@app_home.SwitcherButton(:text => 'Window 1')}                                                                     
	    verify(){@app_home.SwitcherButton(:text => 'Window 2')}                                                                     
	    verify(){@app_home.SwitcherButton(:text => 'Window 3')}                                                                    

	    @app_home.SwitcherButton(:text => 'Window 1').tap
	    @app.MApplicationWindow( :name => 'Window 1' ).MButton( :name => 'CloseButton' ).tap
	    sleep(1)
	    @app_home.SwitcherButton(:text => 'Window 2').tap
	    @app.MApplicationWindow( :name => 'Window 2' ).MButton( :name => 'CloseButton' ).tap
	    sleep(1)
	    @app_home.SwitcherButton(:text => 'Window 3').tap 
	    @app.MApplicationWindow( :name => 'Window 3' ).MButton( :name => 'CloseButton' ).tap
	    sleep(1)
	    
	    #verifies that the multiwindow application is a lazy-shudown application and has the same pid after it is closed
	    newid = string = `pgrep #{@appname}`
	    verify_true(30,"The application is not prestarted"){pid == newid}
        end
    end   

    def test_mw1_content                                                         
	#To test that multiwindow gets the correct window content
        @appname = 'fali_mw1'

        start_exec()
	run_dbus_command_multiwindow('fali_mw1', '1')
	run_dbus_command_multiwindow('fali_mw1', '2')
	run_dbus_command_multiwindow('fali_mw1', '3')

	pid = string = `pgrep #{@appname}`
	@app = @sut.application(:name => @appname) 

        assert_equal("Window 3", @app.MApplicationWindow( :name => 'Window 3' ).MainPage.MLabel.attribute('text'), "Wrong Window Content")
	run_dbus_command_multiwindow('fali_mw1', '1')
        assert_equal("Window 1", @app.MApplicationWindow( :name => 'Window 1' ).MainPage.MLabel.attribute('text'), "Wrong Window Content")
	run_dbus_command_multiwindow('fali_mw1', '2')
        assert_equal("Window 2", @app.MApplicationWindow( :name => 'Window 2' ).MainPage.MLabel.attribute('text'), "Wrong Window Content")
	run_dbus_command_multiwindow('fali_mw1', '3')
        assert_equal("Window 3", @app.MApplicationWindow( :name => 'Window 3' ).MainPage.MLabel.attribute('text'), "Wrong Window Content")
	run_dbus_command_multiwindow('fali_mw1', '2')
        assert_equal("Window 2", @app.MApplicationWindow( :name => 'Window 2' ).MainPage.MLabel.attribute('text'), "Wrong Window Content")

    end        

    def test_close_event
	#Test to hide and close a prestarted application
        @appname = 'close-event'

        start_exec()
        run_dbus_command_closeevent

	@app = @sut.application(:name => 'close-event')
        @app.MComboBox.MLabel( :name => 'CommonTitle' ).tap
	@app.MList.MLabel(:text => 'Ignore closeEvent').tap
        sleep 2
	@app.MButton( :name => 'CloseButton' ).tap		

	pid = string = `pgrep close-event`
        @app.MComboBox.MLabel( :name => 'CommonTitle' ).tap
	@app.MList.MLabel(:text => 'Hide window (Lazy Shutdown)').tap
        sleep 2
	@app.MButton( :name => 'CloseButton' ).tap		
	
        run_dbus_command_closeevent
	newid = string = `pgrep close-event` 
	verify_true(30,"The application is not prestarted"){pid == newid}

        @app.MComboBox.MLabel( :name => 'CommonTitle' ).tap
	@app.MList.MLabel(:text => 'Really close window').tap
        sleep 2
	@app.MButton( :name => 'CloseButton' ).tap
        assert_not_equal(true, @sut.application.test_object_exists?("MLabel", { :name => 'MComboBoxSubtitle'}), "Window still visible")
	pid_now = string = `pgrep close-event`
	verify_true(30,"The application is not prestarted"){newid == pid_now}
    end	
    
    def test_multi_instance_prestart
        #Test to prestart n number of applications at the same time
        @appname = 'fali_multiapp'
        for i in 1..MULTI_APP_PRESTART_TIME
            if $path.include?("scratchbox")
                system "#{@appname} #{i} -prestart -software&"   
            else
                system "#{@appname} #{i} -prestart &"   
            end
            sleep 2
        end
        pid = string = `pgrep #{@appname}` 
        pidlist = pid.split("\n")
        no_of_app = pidlist.length
        assert_equal(MULTI_APP_PRESTART_TIME, no_of_app, "All applications were not prestarted")
        system "pkill #{@appname}"

    end

    def start_exec()
        #function to prestart the applications
        if $path.include?("scratchbox")
            system "export DISPLAY=:1; source /tmp/session_bus_address.user; #{@appname} -prestart -software &"
        else
            system "export DISPLAY=:0; source /tmp/session_bus_address.user; #{@appname} -prestart &"
        end
        sleep 3
    end 

    def run_dbus_command_multiwindow(ename, num)
        #function that calls the dbus-send for each multiplewindow application
        if $path.include?("scratchbox")
	    string = `export DISPLAY=:1; source /tmp/session_bus_address.user; dbus-send --dest=com.nokia.#{ename} --type="method_call" / com.nokia.MultipleWindowsIf.launchWindow int32:"#{num}"`
        else
	    string = `export DISPLAY=:0; source /tmp/session_bus_address.user; dbus-send --dest=com.nokia.#{ename} --type="method_call" / com.nokia.MultipleWindowsIf.launchWindow int32:"#{num}"`
        end
    end

    def run_dbus_command_closeevent()
        #function that calls the dbus-send for close-event application
        if $path.include?("scratchbox")
	    string = `export DISPLAY=:1; source /tmp/session_bus_address.user; dbus-send --dest=com.nokia.#{@appname} --type="method_call" /org/maemo/m com.nokia.MApplicationIf.launch`
        else
	    string = `export DISPLAY=:0; source /tmp/session_bus_address.user; dbus-send --dest=com.nokia.#{@appname} --type="method_call" /org/maemo/m com.nokia.MApplicationIf.launch`
        end
    end

    def kill_application()
        #function to kill the running applications
        puts "going to kill application #{@appname}"
        system "kill -9 `pgrep #{@appname}`"
        sleep 1
    end
end
