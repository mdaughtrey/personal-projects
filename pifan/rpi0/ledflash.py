#!/usr/bin/env python3

from gpiozero import LED

led = LED(14)
led.blink(background=False)
