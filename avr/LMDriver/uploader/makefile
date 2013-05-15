OBJS=uploader.o

uploader: makefile $(OBJS)
	gcc -o uploader $(OBJS)

%.o: %.c
	gcc -g -c $<

run:    	
	./uploader ../interpreter/sconst.pk.bin    	

#run:
#	./interpreter obj.bin

#gdb:
#	gdb interpreter

clean:
	rm *.o
