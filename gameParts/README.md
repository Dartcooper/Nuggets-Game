# Game parts

This is a library containing two modules:
* Player, which stores the address, gold count, name, and letter for a player.
* Grid, which is a two-dimensional array of gridPoints, which contain info regarding the object, objectOnTop (gold pile or player), gold count, and whether each player has seen/can see this point. Also handles converting a map text file into a grid structure and vice versa, as well as randomly placing players/gold piles.

Note: the player testing module requires you run two different servers, both on the same server that this program is being run (plank, babylon1, etc.) The grid testing module requires you run five. These servers need not be run at the same time, however. There are text prompts asking for the port numbers, but they are sent to playertesting.out and servertesting.out respectively.
