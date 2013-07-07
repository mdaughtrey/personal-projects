TARGET=vmhost
OBJS=embedvmhost.o embedvm.o decoder.o
DEFINES=-DDEBUGOUT
INCLUDES=-I. -I..
EVMCOMP=trunk/tools/evmcomp
TEST=simple
#TEST=spiral
#TEST=inctest
TESTEVM=${TEST}.evm
TESTEVMBIN=${TEST}.bin
TESTEVMSYM=${TEST}.sym
OFFSET=$(shell grep main ${TESTEVMSYM} | cut -d' ' -f1)
GDBINIT=$(shell cat.gdbinit | grep -v run | )


${TARGET}: makefile $(OBJS)
	gcc -O0 -g -o ${TARGET} $(OBJS)

%.o: %.c
	gcc -O0 -g ${INCLUDES} -c $(DEFINES) $<

comp:
	${EVMCOMP} ${TESTEVM}

run:
	echo OFFSET is ${OFFSET}
	${EVMCOMP} ${TESTEVM}
#./${TARGET} ${TESTEVMBIN} ${OFFSET} 2>&1 | tee log.txt
	./${TARGET} ${TESTEVMBIN} ${OFFSET}

gdb:
	echo OFFSET is ${OFFSET}
	gdb ${TARGET}

test:
	echo ${OFFSET}


clean:
	-rm *.o
	-rm ${TARGET}
	-rm ${TEST}.{sym,ihx,hdr,dbg,bin,ast}
