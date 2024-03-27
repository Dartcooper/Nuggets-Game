# Makefile for 'client' module
#

OBJS = client.o ../support/log.o ../support/message.o 
L = ../support
LCS50 = ../libcs50
LLIBS = $L/support.a -lncurses
LCS50LIBS = $(LCS50)/libcs50-given.a

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$(LCS50)
CC = gcc
MAKE = make

client: client.o $(LLIBS) $(LCS50LIBS)
	$(CC) $(CFLAGS) $^ -o $@

client.o: client.c

.PHONY: test clean

test:
	bash -v testing.sh &> testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f client
	rm -f core
