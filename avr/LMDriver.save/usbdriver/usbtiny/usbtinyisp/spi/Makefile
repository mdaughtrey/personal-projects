# ======================================================================
# Makefile for USB SPI interface
#
# Copyright (C) 2006 Dick Streefland
#
# This is free software, licensed under the terms of the GNU General
# Public License as published by the Free Software Foundation.
# ======================================================================

USBTINY		= ../usbtiny
TARGET_ARCH	= -mmcu=attiny2313
OBJECTS		= main.o
FLASH_CMD	= avrdude -c avrisp -pt2313 -U flash:w:main.hex
FUSE_CMD	= avrdude -c avrisp -pt2313 -U hfuse:w:0xdb:m -U lfuse:w:0xef:m
STACK		= 32
FLASH		= 2048
SRAM		= 128
SCHEM		= spi.ps

include $(USBTINY)/common.mk
