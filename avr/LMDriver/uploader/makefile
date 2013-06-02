OBJS=uploader.o

uploader: makefile $(OBJS)
	gcc -o uploader $(OBJS)

%.o: %.c
	gcc -DDEBUGOUT -g -c $<

run:
	../interpreter/poke.py bubbles.pk
	./uploader bubbles.pk.bin

#run:    	
#	../interpreter/poke.py ../interpreter/spiral.pk
#	./uploader ../interpreter/spiral.pk.bin

#run:
#	./interpreter obj.bin

#gdb:
#	gdb interpreter

clean:
	rm *.o
