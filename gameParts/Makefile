# Makefile for 'gameparts' library used in nuggets game
#
# Riley Johnson, March 2023

L = ../libcs50
SL = ../support

# Object files, and target library
LIB = gameparts.a
LLIB = $L/libcs50-given.a
OBJS = player.o grid.o $(LLIB)
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(SL) -I$L
CC = gcc

.PHONY: all test clean

all: $(LIB) # test

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

playertest: playertest.o $(LIB) $(LLIB) $(SL)/support.a
	$(CC) $(CFLAGS) $^ -o $@

gridtest: gridtest.o $(LIB) $(LLIB) $(SL)/support.a
	$(CC) $(CFLAGS) $^ -lm -o $@

# Dependencies: object files depend on header files
player.o: player.h $L/mem.h
playertest.o: player.h $L/mem.h $L/file.h $(SL)/message.h
grid.o: grid.h player.h $L/mem.h $L/file.h $L/counters.h $L/set.h $(SL)/message.h
gridtest.o: player.h grid.h $L/file.h

test: playertest gridtest
	valgrind --leak-check=full --show-leak-kinds=all ./playertest &> playertesting.out
	valgrind --leak-check=full --show-leak-kinds=all ./gridtest &> gridtesting.out

clean:
	rm -f *~ *.o *.a
	rm -f playertest gridtest
	rm -f core
