OBJS=uploader.o

uploader: makefile $(OBJS)
	gcc -o uploader $(OBJS)

%.o: %.c
	gcc -DDEBUGOUT -g -c $<

run:    	
	./uploader ../interpreter/clock.pk.bin    	

#run:
#	./interpreter obj.bin

#gdb:
#	gdb interpreter

clean:
	rm *.o
