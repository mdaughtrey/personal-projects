DEVICE=169
TARGET=tempmon
HWTARGET=atmega$(DEVICE)
INCLUDES=-I. -I/usr/avr/include
LIBPATH=-L/usr/avr/lib
CC=avr-gcc
OBJDUMP=avr-objdump
CFLAGS=-mmcu=$(HWTARGET) -Os 
OBJCOPY=avr-objcopy
MAKEFILE=makefile
DEFINES=-DF_CPU=8000000 


LIBS=$(LIBPATH) -lc
OBJS=main.o dmc40457.o w3100a.o

all: clean $(TARGET).hex

$(TARGET).hex: $(MAKEFILE) $(TARGET).out
	$(OBJCOPY) -j .text -O ihex $(TARGET).out $(TARGET).hex

$(TARGET).out: $(MAKEFILE) $(OBJS)
	$(CC) -o $(TARGET).out $(CFLAGS) -Wl,-Map,$(TARGET).map $(OBJS) $(LIBS)

writefuse: 
	avrdude -v -qq -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE) -Uhfuse:w:0xd9:m
	avrdude -v -qq -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE) -Ulfuse:w:0xe2:m

readfuse:
	avrdude -vv -qq -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE) -Uhfuse:r:high.txt -Ulfuse:r:low.txt
	cat high.txt low.txt

erase:
	avrdude -vv -qq -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE) -e 

boardtest: 
	avrdude -qqq -P -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE)

clean:
	rm -f *.o *.out *.map *.hex

upload:
	avrdude -qq -C /etc/avrdude.conf -c usbtiny -p m$(DEVICE) -U flash:w:$(TARGET).hex

%.o: %.c
	$(CC) -c -g $(CFLAGS) $(DEFINES) $(INCLUDES) $<
	$(OBJDUMP) -Sh $@ > $<.lst
#	$(CC) -Os -c $(CFLAGS) $(DEFINES) $(INCLUDES) $<
