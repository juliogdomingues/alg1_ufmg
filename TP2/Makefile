CC = gcc
LIBS = -lstdc++
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/main.o
# HDRS = $(INC)/heap.hpp
CFLAGS = -g -Wall -std=c++17 -c -I$(INC)

EXE = $(BIN)/main.out

all: $(EXE)

tests: $(EXE)
	$(EXE) -d < tests/test1.txt
	$(EXE) -d < tests/test2.txt
	$(EXE) -d < tests/test3.txt
	
$(BIN)/main.out: $(OBJS)
	$(CC) -o $(BIN)/main.out $(OBJS) $(LIBS)

$(OBJ)/main.o: $(HDRS) $(SRC)/main.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/main.o $(SRC)/main.cpp 
	
clean:
	rm -f $(EXE) $(OBJS)