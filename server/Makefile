# Makefile for 'indexer' module
#
# Jorie MacDonald, January 2023

OBJS = server.o ../support/message.o ../support/log.o ../gameParts/grid.o
LIBS = ../libcs50/libcs50-given.a ../gameParts/gameparts.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I ../libcs50 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

server: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^  -o $@ -lm

# counterstest.o: counters.h ../lib/file.h
server.o: server.c ../gameParts/grid.h
message.o: ../support/message.h ../support/log.h
log.o: ../support/log.h
../gameParts/grid.o: ../gameParts/grid.h 
../gameParts/player.o: ../gameParts/


.PHONY: clean test
# testing

test:
	bash -v testing.sh

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f bagtest
	rm -f core
