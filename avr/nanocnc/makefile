TARGET=projector
HWTARGET=atmega169
INCLUDES=-I. -I/usr/lib/avr/include/avr
LIBPATH=-L/usr/avr/lib
CC=avr-gcc
OBJDUMP=avr-objdump
CFLAGS=-mmcu=$(HWTARGET) -O3
OBJCOPY=avr-objcopy
MAKEFILE=makefile
DEFINES=-DF_CPU=8000000 
#-DEASYDRIVER


LIBS=$(LIBPATH) -lc
OBJS=main.o serutil.o stepper.o


all: clean $(TARGET).hex

$(TARGET).hex: $(MAKEFILE) $(TARGET).out
	$(OBJCOPY) -j .text -O ihex $(TARGET).out $(TARGET).hex

$(TARGET).out: $(MAKEFILE) $(OBJS)
	$(CC) -o $(TARGET).out $(CFLAGS) -Wl,-Map,$(TARGET).map $(OBJS) $(LIBS)

writefuse: 
	avrdude -c avrisp2 -p m169 -P usb -Ulfuse:w:0xc2:m 
	avrdude -c avrisp2 -p m169 -P usb -Uhfuse:w:0xd9:m

erase:
	avrdude -q -P /dev/ttyS0 -p m169 -c stk500v2 
boardtest: 
	avrdude -P /dev/ttyS0 -p m169 -c stk500v2

clean:
	rm -f *.o *.out *.map *.hex

upload:
	avrdude -c avrisp2 -p m169 -P usb -U flash:w:$(TARGET).hex

%.o: %.c
	$(CC) -c -g $(CFLAGS) $(DEFINES) $(INCLUDES) $<
	$(OBJDUMP) -Sh $@ > $<.lst
#	$(CC) -Os -c $(CFLAGS) $(DEFINES) $(INCLUDES) $<
