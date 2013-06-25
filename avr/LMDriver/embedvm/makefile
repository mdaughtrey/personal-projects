TARGET=vmhost
OBJS=embedvmhost.o embedvm.o
DEFINES=-DDEBUGOUT
INCLUDES=-I
EVMCOMP=trunk/tools/evmcomp
TESTEVM=test_strings.evm
TESTEVMBIN=test_strings.bin

${TARGET}: makefile $(OBJS)
	gcc -O0 -g -o ${TARGET} $(OBJS)

%.o: %.c
	gcc -O0 -g ${INCLUDES} -c $(DEFINES) $<

run:
	${EVMCOMP} ${TESTEVM}
	./${TARGET} ${TESTEVMBIN}

gdb:
	gdb ${TARGET}


clean:
	-rm *.o
	-rm ${TARGET}
