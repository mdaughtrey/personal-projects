TARGET=ppspi
INCLUDES=-I. -I/usr/include
LIBPATH=-L/usr/lib
CC=gcc
CPP=g++
CFLAGS=-g -c
CPPFLAGS=-g -c -fexceptions
LINKFLAGS=
MAKEFILE=makefile
DEFINES=

LIBS=$(LIBPATH) -lstdc++ -llog4cpp -lrt
OBJS=ppspi.o buildlogger.o parallelport.o programmer.o Interpreter.o spitransaction.o

$(TARGET): $(MAKEFILE) $(OBJS)
	$(CC) -o $(TARGET) $(LINKFLAGS) $(OBJS) $(LIBS)


%.o: %.c 
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) $<

%.o: %.cpp 
	$(CPP) $(CPPFLAGS) $(DEFINES) $(INCLUDES) $<

