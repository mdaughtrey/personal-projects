OBJS=interpreter.o
DEFINES=-DDEBUGOUT

interpreter: makefile $(OBJS)
	gcc -O0 -g -o interpreter $(OBJS)

%.o: %.c
	gcc -O0 -g -c $(DEFINES) $<

run:
	./interpreter clock.pk.bin

gdb:
	gdb ./interpreter

poke:
	./poke.py -v 

clean:
	rm *.o
	rm interpreter
