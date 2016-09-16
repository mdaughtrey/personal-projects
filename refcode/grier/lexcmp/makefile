CXX=g++-4.6
CXXFLAGS=-g -std=c++0x -Wall -pedantic -I.
LDFLAGS=-lcrypto
BIN=a.out

SRC=$(wildcard *cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) ${LDFLAGS} -o $(BIN) $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)

run:
	./$(BIN) 

gdb:
	gdb $(BIN)
