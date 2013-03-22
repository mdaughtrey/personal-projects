OBJS=interpreter.o

interpreter: makefile $(OBJS)
	gcc -o interpreter $(OBJS)

%.o: %.c
	gcc -g -c $<

run:
	./interpreter obj.bin

gdb:
	gdb interpreter

poke:
	./poke.py -v
