# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import gc
import webrepl
#import server
webrepl.start()
gc.collect()
#xx=server.Server()
#xx.go()
