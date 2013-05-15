#!/usr/bin/env python
"""
This program tests the startup time of the given application with and
without prestarting.

Requirements:
1. DISPLAY environment variable must be set correctly.
2. DBus session bus must be running.
3. DBus session bus address must be stored in /tmp/session_bus_address.user.
4. Given application supports prestarting with -prestart commandline argument.

Usage:    test-perf-prestart <prestartable application>

Example:  test-perf-prestart /usr/bin/camera-ui

Authors: antti.kervinen@nokia.com, juha.lintula@nokia.com
"""
import os
import subprocess
import commands
import time
import sys
from optparse import OptionParser
import unittest

MEMORYHOG = '/usr/bin/memoryhog'
DEV_NULL = file("/dev/null","w")
LOG_FILE = '/tmp/applifed_perftest.log'

_timer_pipe = None
_start_time = 0
_memory_stats = []


appname = None
test_mode = None

def parseCommandLineOptions():
    global appname, test_mode
    my_parser = OptionParser()
    my_parser.add_option("-a", "--app", dest="app", help="path of the application you want to test", action="store")
    my_parser.add_option("-s", "--swap", dest="swap", help="Flag for swap mode of testing, if not used, test_mode = Regular", action="store_true", default=False)
    options, arguments = my_parser.parse_args()
    if not options.app:
        print "you must specify the application you want to test"
        sys.exit(1)
    else: 
        assert (os.path.exists(options.app)), "The application %s does not exist" % options.app
        appname = options.app
    if options.swap: 
        test_mode = 'Swap'
    else:
        test_mode = 'Regular'

def debug(*msg):
    sys.stderr.write('[DEBUG %s] %s\n' % (time.time(), ' '.join([str(s) for s in msg]),))

def error(*msg):
    sys.stderr.write('ERROR %s\n' % (' '.join([str(s) for s in msg]),))
    sys.exit(1)

def basename(appname):
    return appname.split('/')[-1]

def is_executable_file(filename):
    return os.path.isfile(filename) and os.access(filename, os.X_OK)
    
def check_prerequisites(appname):
    if os.getenv('DISPLAY') == None:
        error("DISPLAY is not set. Check the requirements.")
        
    if os.getenv('DBUS_SESSION_BUS_ADDRESS') == None:
        error("DBUS_SESSION_BUS_ADDRESS is not set.\n" +
              "You probably want to source /tmp/session_bus_address.user")

    if not is_executable_file(appname):
        error("'%s' is not an executable file\n" % (appname,) +
              "(should be an application that supports prestarting)")

class applifed_perf_tests(unittest.TestCase):
    def setUp(self):
        self.tcname = self.id().split('.')[-1]
        debug("Stopping applifed")
        os.system("initctl stop xsession/applifed")

    def tearDown(self):
        os.system("initctl start xsession/applifed")
        st, pid = commands.getstatusoutput("pgrep applifed")
        debug("applifed started again with pid : %s" %pid)
        pass
        #print "End %s" % self.tcname
    
    def start_timer(self):
	# call measure_time after calling this...
	global _start_time
	_start_time = time.time()

    def measure_time(self):
	global _end_time
	fh = open(LOG_FILE, "r")
	lines = fh.readlines()
	lastline = lines[len(lines)-2]
	_end_time = lastline.split()[0]
	debug("End Time :%f" %float(_end_time))
	_app_start_time = float(_end_time) - float(_start_time)
	return _app_start_time

    def save_memory_usage(self, process_handle):
	global _memory_stats
	rss, private_dirty = 0, 0
	if process_handle:
	    # calculate process memory usage
	    # resident size in memory and private dirty
	    smaps_lines = file("/proc/%s/smaps" % (process_handle.pid,)).readlines()
	    for line in smaps_lines:
		if line.startswith("Rss:"):
		    rss += int(line.split()[1])
		elif line.startswith("Private_Dirty:"):
		    private_dirty += int(line.split()[1])
	# find out free system memory
	meminfo_lines = file("/proc/meminfo").readlines()
	for line in meminfo_lines:
	    if line.startswith("MemFree:"):
		memfree = int(line.split()[1])
	    elif line.startswith("Buffers:"):
		memfree += int(line.split()[1])
	    elif line.startswith("Cached:"):
		memfree += int(line.split()[1])
	_memory_stats.append({'rss':rss,
			      'private_dirty': private_dirty,
			      'memfree': memfree})

    def eat_memory(self):
	debug("Running memoryhog...")

	p = subprocess.Popen(MEMORYHOG,
			 stdout=subprocess.PIPE)
	# Memoryhog is ready when we are able to read 3 lines from it.
	debug ("Memoryhog: %s"%p.stdout.readline())
	debug ("Memoryhog: %s"%p.stdout.readline())
	debug ("Memoryhog: %s"%p.stdout.readline())
	return

    def run_without_prestarting(self, appname, test_mode):
	"""returns process handle with pid attribute and terminate function"""
        os.system ('mcetool --set-tklock-mode=unlocked')
	if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):
			    os.system('rm %s' %LOG_FILE)
	if (test_mode == "Swap"):
	    self.eat_memory()

	self.start_timer()
	p = subprocess.Popen(appname,
			     shell=False,
			     stdout=DEV_NULL, stderr=DEV_NULL)
	debug("app", appname, "started")
	return p

    def run_without_prestarting_without_meegotouchhome(self, appname, test_mode):
	"""returns process handle with pid attribute and terminate function"""
        os.system ('mcetool --set-tklock-mode=unlocked')
	if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):
			    os.system('rm %s' %LOG_FILE)
	if (test_mode == "Swap"):
	    self.eat_memory()
	os.system('pkill -STOP meegotouchhome')
	self.start_timer()
	p = subprocess.Popen(appname,
			     shell=False,
			     stdout=DEV_NULL, stderr=DEV_NULL)
	debug("app", appname, "started without meegotouchhome")
	os.system('pkill -CONT meegotouchhome')
	return p

    def run_with_prestarting(self, appname, test_mode):
	"""returns process handle with pid attribute and terminate function"""
	# for lifecycle application, for instance, run
	# dbus-send --dest=com.nokia.lifecycle \
	#           --type="method_call" \
	#           /org/maemo/m com.nokia.MApplicationIf.launch
        os.system ('mcetool --set-tklock-mode=unlocked')
	global _start_time
	if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):
			    os.system('rm %s' %LOG_FILE)
	time.sleep(1)
	output = commands.getoutput('dbus-send --dest=com.nokia.' + basename(appname) +
				    ' --type="method_call" /org/maemo/m ' +
				    'com.nokia.MApplicationIf.ping')
	debug("app prestarted, waiting for it to stabilize...")
	time.sleep(3)
	class p: pass
	p.pid = commands.getoutput('pgrep ' + basename(appname)[:15])
	# Make sure the application is prestarted and ready to receive the
	# launch signal. Hypothesis: if the application has not been
	# scheduled within the second, it is ready for launching
	
	#Commented, needs investigation why this waits when meegotouchhome stopped
	"""switches = 0
	last_switches = -100
	counter = 0
	while (switches - last_switches) > 0 and counter < 20:
	    debug("still waiting, switches, last_switches:...", switches, last_switches)
	    time.sleep(1)
	    counter = counter +1
	    last_switches = switches
	    switches = int(commands.getoutput("grep nr_switches /proc/%s/sched" %
					  (p.pid,)).split()[2])"""
	self.save_memory_usage(p)

	# If we testing the the worst case, make sure that the prestarted is swapped off
	if (test_mode == "Swap"):
	    self.eat_memory()
	
	# Launch the prestarted application and quit immediately so that
	# the time can be measured
	self.start_timer()
	debug("start time : %f"% _start_time)
	output = commands.getoutput('dbus-send --dest=com.nokia.' + basename(appname) +
				    ' --type="method_call" /org/maemo/m ' +
				    'com.nokia.MApplicationIf.launch')
	return p

    def run_with_prestarting_without_meegotouchhome(self, appname, test_mode):
	"""returns process handle with pid attribute and terminate function"""
	# for lifecycle application, for instance, run
	# dbus-send --dest=com.nokia.lifecycle \
	#           --type="method_call" \
	#           /org/maemo/m com.nokia.MApplicationIf.launch
        os.system ('mcetool --set-tklock-mode=unlocked')
	global _start_time
	if os.path.exists(LOG_FILE) and os.path.isfile(LOG_FILE):
			    os.system('rm %s' %LOG_FILE)
	time.sleep(1)
	output = commands.getoutput('dbus-send --dest=com.nokia.' + basename(appname) +
				    ' --type="method_call" /org/maemo/m ' +
				    'com.nokia.MApplicationIf.ping')
	debug("app prestarted, waiting for it to stabilize...")
	time.sleep(3)
	class p: pass
	p.pid = commands.getoutput('pgrep ' + basename(appname)[:15])
	# Make sure the application is prestarted and ready to receive the
	# launch signal. Hypothesis: if the application has not been
	# scheduled within the second, it is ready for launching
	switches = 0
	last_switches = -100
	counter = 0
	#Commented, needs investigation why this waits when meegotouchhome stopped
	"""while (switches - last_switches) > 0 and counter < 30:
	    debug("still waiting, switches, last_switches:...", switches, last_switches)
	    time.sleep(1)
	    counter = counter +1
	    last_switches = switches
	    switches = int(commands.getoutput("grep nr_switches /proc/%s/sched" %
					  (p.pid,)).split()[2])"""
	self.save_memory_usage(p)

	# If we testing the the worst case, make sure that the prestarted is swapped off
	if (test_mode == "Swap"):
	    self.eat_memory()
	
	# Launch the prestarted application and quit immediately so that
	# the time can be measured
	os.system('pkill -STOP meegotouchhome')
	self.start_timer()
	debug("start time : %f"% _start_time)
	output = commands.getoutput('dbus-send --dest=com.nokia.' + basename(appname) +
				    ' --type="method_call" /org/maemo/m ' +
				    'com.nokia.MApplicationIf.launch')
	debug("prestarted app launched without meegotouchhome")
	os.system('pkill -CONT meegotouchhome')
	return p


    def kill_process(self, process_handle, appname):
	return commands.getoutput("pkill -9 %s" % (basename(appname)[:15],))

    def perftest_with_prestart(self, appname, test_mode):
	debug("run app with prestarting with meegotouchhome")
	self.save_memory_usage(None) # record free system memory
	p = self.run_with_prestarting(appname, test_mode)
	time.sleep(2)
	time_with_prestart = self.measure_time()
	time.sleep(2)
	self.save_memory_usage(p)
	self.kill_process(p, appname)
	if (test_mode=="Swap"):  commands.getoutput("pkill -9 memoryhog")
	debug("got time:", time_with_prestart)
	time.sleep(2)
	
	debug("run app with prestarting without meegotouchhome ")
	self.save_memory_usage(None) # record free system memory
	p = self.run_with_prestarting_without_meegotouchhome(appname, test_mode)
	time.sleep(2)
	time_with_prestart_without_meegotouchhome = self.measure_time()
	time.sleep(2)
	self.save_memory_usage(p)
	self.kill_process(p, appname)
	if (test_mode=="Swap"):  commands.getoutput("pkill -9 memoryhog")
	debug("got time:", time_with_prestart_without_meegotouchhome)
	time.sleep(2)
	return time_with_prestart, time_with_prestart_without_meegotouchhome
	
    def perftest_without_prestart(self, appname, test_mode):
	debug("run app without prestarting with meegotouchhome")
	self.save_memory_usage(None) # record free system memory
	p = self.run_without_prestarting(appname, test_mode)
	time.sleep(4)
	time_without_prestart = self.measure_time()
	self.save_memory_usage(p)
	time.sleep(4)
	self.kill_process(p, appname)
	if (test_mode=="Swap"):  commands.getoutput("pkill -9 memoryhog")
	debug("got time:", time_without_prestart)
	time.sleep(2)
	
	debug("run app without prestarting without meegotouchhome")
	self.save_memory_usage(None) # record free system memory
	p = self.run_without_prestarting_without_meegotouchhome(appname, test_mode)
	time.sleep(4)
	time_without_prestart_without_meegotouchhome = self.measure_time()
	self.save_memory_usage(p)
	time.sleep(4)
	self.kill_process(p, appname)
	if (test_mode=="Swap"):  commands.getoutput("pkill -9 memoryhog")
	debug("got time:", time_without_prestart_without_meegotouchhome)
	time.sleep(2)
	
	return time_without_prestart, time_without_prestart_without_meegotouchhome

    def print_test_report(self, with_without_times, fileobj):
	"""
	with_without_times is a list of pairs:
	   (with_prestarting_startup_time,
	    without_prestarting_startup_time)
	"""
	global _memory_stats
	def writeline(*msg):
	    fileobj.write("%s\n" % ' '.join([str(s) for s in msg]))
	def fmtfloat(f):
	    return "%.2f" % (f,)
	def filterstats(data, field):
	    return tuple([d[field] for d in data])

	if with_without_times == []: return

	writeline("")
	writeline('Memory consumptions [kB]:')
	memrowformat = "%10s %10s %10s %10s %10s %10s"
	while _memory_stats:
	    oneround = _memory_stats[:5]
	    _memory_stats = _memory_stats[5:]
	    
	    writeline(memrowformat %
		      ('', 'before', 'before', 'after', 'before', 'after'))
	    writeline(memrowformat %
		      ('', 'prestart', 'launch', 'launch', 'normal', 'normal'))
	    writeline(memrowformat %
		      (('privdirt',) + filterstats(oneround, 'private_dirty')))
	    writeline(memrowformat %
		      (('rss',) + filterstats(oneround, 'rss')))
	    writeline(memrowformat %
		      (('free',) + filterstats(oneround, 'memfree')))
	    writeline("")

	writeline("")
	rowformat = "%12s %12s %12s %12s"
	writeline('Startup times [s]:')
	writeline(rowformat % ('prestarted-Yes', 'prestarted-Yes', 'prestarted-No', 'prestarted-No'))
	writeline(rowformat % ('meegotouchhome-Yes   ', 'meegotouchhome-No    ', 'meegotouchhome-Yes  ', 'meegotouchhome-No   '))
       
	t1,t2,t3,t4 = [], [], [], []
	for wop_wd, wop_wod, wp_wd, wp_wod in with_without_times:
	    t1.append(wop_wd)
	    t2.append(wop_wod)
	    t3.append(wp_wd)
	    t4.append(wp_wod)
	    writeline(rowformat % (fmtfloat(wop_wd), fmtfloat(wop_wod),
				  fmtfloat(wp_wd), fmtfloat(wp_wod)))

	writeline('Average times:')
	writeline(rowformat % (fmtfloat(sum(t1)/len(t1)),fmtfloat(sum(t2)/len(t2)),
			      fmtfloat(sum(t3)/len(t3)),fmtfloat(sum(t4)/len(t4))))
	return fmtfloat(sum(t1)/len(t1))

    def test_001(self):
	times = []
	times1, times2 = [], []
	
	for i in xrange(3):
		times1.append(self.perftest_with_prestart(appname, test_mode))
	for i in xrange(3):
		times2.append(self.perftest_without_prestart(appname, test_mode))
	
	times = [[t1[0], t1[1], times2[i][0], times2[i][1]] for i, t1 in enumerate(times1)]
	avg_with_prestart = self.print_test_report(times, sys.stdout)
	self.assert_(float(avg_with_prestart) < float(0.75), "prestarted application takes more than 0.75 sec")


# main
if __name__ == '__main__':
    parseCommandLineOptions()
    print sys.argv
    sys.argv = sys.argv[0:1]
    print sys.argv
    check_prerequisites(appname)
    unittest.main()
    
