#!/bin/env python3

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont
import sys

disp = Adafruit_SSD1306.SSD1306_128_64(rst=0)

def disp_status(text):
    disp.clear()
    image = Image.new('1', (disp.width, disp.height))
    draw = ImageDraw.Draw(image)
    font = ImageFont.load_default()
    draw.multiline_text((0,-2), text, font=font, fill=255)
    disp.image(image)
    disp.display()


def main():
    disp.clear()
    disp_status("Shutdown")
    sys.exit(0)

main()
