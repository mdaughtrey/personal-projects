# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import uos, machine
#uos.dupterm(None, 1) # disable REPL on UART(0)
import gc
import network
import webrepl
import utime
gc.collect()

#global sta_if
#sta_if = network.WLAN(network.STA_IF);
#sta_if.active(True)
#sta_if.scan()                             # Scan for available access points
#sta_if.connect("yfinity", "24HoursADay") # Connect to an AP
#sta_if.config(dhcp_hostname='qclock')
#while False == sta_if.isconnected():
#    print("Waiting to connect")
#    utime.sleep(1)
#webrepl.start()
#import qclock
#qclock.main()

