TARGET=5x7single
HWTARGET=at90s4433
INCLUDES=-I. -I/usr/avr/include
LIBPATH=-L/usr/avr/lib
CC=avr-gcc
CFLAGS=-mmcu=$(HWTARGET)
OBJCOPY=avr-objcopy
MAKEFILE=makefile
DEFINES=
LIBS=$(LIBPATH) -lc
OBJS=main.o serutil.o font5x7.o cmdproc.o displaymux.o

all: clean $(TARGET).hex upload

$(TARGET).hex: $(MAKEFILE) $(TARGET).out
	$(OBJCOPY) -j .text -O ihex $(TARGET).out $(TARGET).hex

$(TARGET).out: $(MAKEFILE) $(OBJS)
	$(CC) -o $(TARGET).out $(CFLAGS) -Wl,-Map,$(TARGET).map $(OBJS) $(LIBS)

boardtest: 
	uisp -v -dprog=stk500 -dserial=/dev/ttyS0 -dpart=$(HWTARGET) --rd_osc

fuses: 
	uisp -v -dprog=stk500 -dserial=/dev/ttyS0 -dpart=$(HWTARGET) --rd_fuses

clean:
	rm -f *.o *.out *.map *.hex

upload:

	uisp -v  -dprog=stk500 -dserial=/dev/ttyS0 -dpart=$(HWTARGET) --erase --upload --verify if=$(TARGET).hex

%.o: %.c
	$(CC)  -c $(CFLAGS) $(DEFINES) $(INCLUDES) $<
#	$(CC) -Os -c $(CFLAGS) $(DEFINES) $(INCLUDES) $<
