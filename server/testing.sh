#!/bin/bash
#
# Test code for server program for nuggets game
# All output goes to stdout/stderr
#
# Riley Johnson, March 2023

# Invalid arguments
# No arguments
./server

# Three arguments
./server ../maps/main.txt 10 toomuch

# Invalid map file (invalid path)
./server ../maps/notreal.txt 

# Invalid seed (non-positive int)
./server ../maps/main.txt -10

# Valid run on main.txt with valgrind, ensuring the following functionality:
# Joining with a client with a blank name is rejected.
#
# Walls stop motion, sides of passages can't be walked through, end of a
# dead-end passage does not lead into empty space.
#
# Visibility functions as normal; clients can only see rooms and passages they
# have already been in, can only see the contents of areas they have line of
# sight with, passages are "dark" and only one space ahead at a time can
# be seen.
#
# Gold can be picked up, and the proper message is delivered when it is.
#
# Two players can see each other (but only when they are in line of sight of
# each other), when one player picks up gold it disappears on both their and
# the other player's screen, walking into each other swaps positions.
#
# Join with a spectator; make sure they can see everything, including updates
# to players' movements. When another spectator joins, the current one should
# be kicked.
valgrind --leak-check=full --show-leak-kinds=all ./server ../maps/main.txt

# Print last exit value; should be 0
echo "Exit value was $?"

# Run twice with a pre-chosen seed; check that gold pile placements/amounts and
# player spawns are the same. Done on visdemo.txt for the sake of time.
./server ../maps/visdemo.txt 10
./server ../maps/visdemo.txt 10

# Repeat with random seed; check that gold pile placements/amounts and player
# spawns are different.
./server ../maps/visdemo.txt
./server ../maps/visdemo.txt
