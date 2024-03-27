# Makefile for CS50 Nuggets Lab
#
# Cooper LaPorte - March 2023

L = libcs50
.PHONY: all clean

############## default: make all libs and programs ##########
all: 
	make -C support
	make -C libcs50
	make -C gameParts
	make -C client
	make -C server


############## clean  ##########
clean:
	rm -f *~
	make -C support clean
	make -C libcs50 clean
	make -C gameParts clean
	make -C client clean
	make -C server clean
