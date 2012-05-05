TARGET=rocketbeeper
HWTARGET=attiny45
INCLUDES=-I. -I/usr/avr/include
LIBPATH=-L/usr/avr/lib
CC=avr-gcc
OBJDUMP=avr-objdump
CFLAGS=-mmcu=$(HWTARGET) 
OBJCOPY=avr-objcopy
MAKEFILE=makefile
#DEFINES=-DLTP1157=1
#DEFINES=-DKB_TBC12=1 -DALTFONT=1
DEFINES=-DKB_TBC12=1 \
-DF_CPU=8000000 \

# -DSTEPDISP


LIBS=$(LIBPATH) -lc
OBJS=rocketbeeper.o

all: clean $(TARGET).hex 

$(TARGET).hex: $(MAKEFILE) $(TARGET).out
	$(OBJCOPY) -j .text -O ihex $(TARGET).out $(TARGET).hex

$(TARGET).out: $(MAKEFILE) $(OBJS)
	$(CC) -o $(TARGET).out $(CFLAGS) -Wl,-Map,$(TARGET).map $(OBJS) $(LIBS)

readfuse: 
	avrdude -qq -C /etc/avrdude.conf -c usbtiny -p t45 -Uhfuse:r:hfuse.txt:i
	avrdude -qq -C /etc/avrdude.conf -c usbtiny -p t45 -Ulfuse:r:lfuse.txt:i

mmreadfuse: 
	avrdude -p m169 -c mattsmod -Uhfuse:r:hfuse.txt:i

writefuse: 
	avrdude -qq -C /etc/avrdude.conf -c usbtiny -p t45 -Ulfuse:w:0x42:m

erase:
	avrdude -q -P /dev/ttyS0 -p m169 -c stk500v2 
boardtest: 
	avrdude -P /dev/ttyS0 -p m169 -c stk500v2

clean:
	rm -f *.o *.out *.map *.hex

upload:
#	avrdude -qq -P /dev/ttyS0 -c stk500v2 -p m169 -U flash:w:$(TARGET).hex
	avrdude -qq -C /etc/avrdude.conf -c usbtiny -p t45 -U flash:w:RocketBeeper/$(TARGET).hex

# -Wa,-ahls=$<.lst

%.o: %.c
	$(CC) -c -g $(CFLAGS) $(DEFINES) $(INCLUDES) $<
	$(OBJDUMP) -Sh $@ > $<.lst
#	$(CC) -Os -c $(CFLAGS) $(DEFINES) $(INCLUDES) $<
