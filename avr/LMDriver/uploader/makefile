OBJS=uploader.o

EVM=../embedvm/simple
MAIN_OFFSET=$(shell grep main ${EVM}.sym | cut -d' ' -f1)
#BIN_OFFSET=$(shell grep '(code)$' ${EVM}.sym | head -1 | cut -d' ' -f1)
BIN_OFFSET=$(shell grep "(code)" ${EVM}.sym | head -1 | cut -d' ' -f1)

uploader: makefile $(OBJS)
	gcc -o uploader $(OBJS)

%.o: %.c
	gcc -DDEBUGOUT -g -c $<

run:
	echo MAIN_OFFSET ${MAIN_OFFSET}
	echo BIN_OFFSET ${BIN_OFFSET}
	./uploader ${EVM}.bin ${MAIN_OFFSET} ${BIN_OFFSET}

#run:    	
#	../interpreter/poke.py ../interpreter/spiral.pk
#	./uploader ../interpreter/spiral.pk.bin

#run:
#	./interpreter obj.bin

gdb:
	echo "file uploader" > .gdbinit
	echo "break main" >> .gdbinit
	echo "run ${EVM}.bin ${MAIN_OFFSET} ${BIN_OFFSET}" >> .gdbinit
	gdb

clean:
	rm *.o
