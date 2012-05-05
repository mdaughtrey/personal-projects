TARGET=ppm
MK=makefile
CC=gcc -c -g
CFLAGS=
DEFINES=
LINK=
INCLUDES=-I .
LIBS=
OBJS=main.o nav.o menu.o nvstore.o  ui.o lcd.o funcs.o utils.o console.o

#ui.o lcd.o usbkbd.o eeprom.o fingerscan.o cmdproc.o

$(TARGET): $(MK) $(OBJS)
	gcc -o $(TARGET) $(OBJS) $(LIBS)


%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) $<

clean:
	rm *.o $(TARGET)

initnv:
	dd if=/dev/zero of=nvmemory.mem bs=1024 count=2
