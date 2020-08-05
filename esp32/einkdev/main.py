#import epaper2in13
from  ssd1675a import SSD1675A
import framebuf
import machine
import time
#from image_dark import hello_world_dark

# plain
# b red/black/white
# c yellow/black/white
# d plain flexible

#/ Good Display GDEH0213B72
#/ HINK-E0213A22
#/ IL3897 / ssd16xx SSD1675
#/ waveshare_epaper_gdeh0213b72
#/ https://v4.cecdn.yun300.cn/100001_1909185148/GDEH0213B72.pdf

# Busy IO4
# Reset IO16
# MOSI IO23
# CLK IO18
# DC IO17
# CS IO5
EWIDTH=212
EHEIGHT=128

#pins = [machine.Pin(4, machine.Pin.OUT), machine.Pin(16, machine.Pin.OUT), machine.Pin(17, machine.Pin.OUT), machine.Pin(5, machine.Pin.OUT)]

def main():
    spi = machine.SPI(2, baudrate=2000000, sck=machine.Pin(18, machine.Pin.OUT), mosi=machine.Pin(23, machine.Pin.OUT))
    #def __init__(self, spi, cs_pin, dc_pin, busy_pin, reset_pin, width=296, height=128):
    e = SSD1675A(spi, cs_pin=machine.Pin(5), dc_pin=machine.Pin(17),
        busy_pin=machine.Pin(4), reset_pin=machine.Pin(16),
        width=EWIDTH, height=EHEIGHT)
    e.clear()
#    e.rect(20, 20, 100, 100, 1)
#    e.show()

#    while True:
#        spi.write("ab")
#        time.sleep(1)
#    while True:
#        spi.write("abcd")
#        for p in pins:
#            p.on()
#            time.sleep(0.1)
#        for p in pins:
#            p.off()
#            time.sleep(0.1)

#        mosi=machine.Pin(23))
    #spi.init(baudrate=2000000) # , sck=machine.Pin(18), mosi=machine.Pin(23))
    #def __init__(self, spi, cs, dc, rst, busy):
#    e = epaper2in13.EPD(spi, machine.Pin(5), machine.Pin(17), machine.Pin(16), machine.Pin(4))
#    e.init()
#    print("init done")
#    e.wait_until_idle()
#    print("Idle")

#    e.clear_frame_memory(0)
#    e.set_frame_memory(hello_world_dark, 0, 0, e.width, e.height)
#    e.clear_frame_memory(0)
##    print("clear done")
#    e.display_frame()
#    print("display done")
#    buf = bytearray(EWIDTH, EHEIGHT // 8)
#    buf = bytearray(e.height * e.width // 8)
#    fb = framebuf.FrameBuffer(buf, EHEIGHT, EWIDTH, framebuf.MONO_HLSB)
#    fb.text('Hello', 30, 0, 0)
#    e.show(buf)
#    e.set_frame_memory(buf, 0, 0, e.width, e.height)
#    e.display_frame()
    print("Done")

