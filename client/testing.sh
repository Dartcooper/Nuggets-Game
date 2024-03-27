#!/bin/bash
#
# Test code for client program for nuggets game
# All output goes to stdout/stderror
#
# Riley Johnson, March 2023

# Invalid arguments
# No arguments
./client

# One argument
./client localhost

# Four arguments
./client localhost 1 john toomuch

# Invalid hostname
./client nonsense 1

# Invalid port
./client localhost -1

# The next tests are merely descriptions of what to do, they are not recorded.
# Use the server found in shared.

# Valid run as a player (giving a name) with valgrind, ensuring the
# following functionality:
# Make sure all inputs go through and that your character responds normally
# (test all controls, including diagonals and uppercase variants).
#
# Make sure gold can be collected as normal and that collecting all gold results
# in the game ending with a proper summary (that is, your game should end with 
#
# Ensure both Q and EOF result in the game ending as normal

# Valid run as a spectator (no name); no valgrind necessary, just ensure display
# is that of a spectator.
