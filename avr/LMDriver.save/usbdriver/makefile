TARGET=usbdriver
INCLUDE=
LIB=-lusb -lstdc++
CFLAGS=-g -I.
OBJS=usbdriver.o

$(TARGET): makefile $(OBJS)
	gcc -o $(TARGET) $(OBJS) $(LIB)

%.o: %.cpp
	gcc -c $(CFLAGS) $(DEFINES) $(INCLUDE) $<
