# This file would be placed at /etc/prestart/ and renamed 
# to example.prestart if it was really used

# This defines the prestartable application binary
# with possible argumets
Service=com.nokia.lifecycle

# Applications with priority level less than zero will be prestarted
# immediately, one by one, when applifed has been started and before it
# daemonizes itself. Applications with the smallest priority level will be
# started first.
#
# Applications with priority level greater or equal to zero will be
# prestarted when system load goes down and there is still enough memory.
# In practice, this means that desktop is already up and running.
Priority=1
