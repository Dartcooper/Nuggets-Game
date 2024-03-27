# CS50 Nuggets
## Design Spec
### Breaking Bash, Winter, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

The client has two main interfaces; the arguments it takes, and keystrokes, which it passes to the server. The arguments are below:

```
$ client hostname port [playername]
```

Where: 
- Hostname is the IP address where the server is running
- Port is the port number the server expects messages on (integer between 0 and 65536)
- Playername is optional and is the real name the server will print out with the results at the end of the game; if not present, client runs as a spectator

For both spectator and player clients, q (or end of file) quits the game.

For player clients:
- h moves left
- l moves right
- J moves down
- k moves up
- y moves up and to the left
- u moves up and to the right
- b moves down and to the left
- n moves down and to the right

The capitalized version of any of the above input causes movement in that direction until an unwalkable area is reached (wall, end of tunnel).



### Inputs and outputs

Input: there are no file inputs; there are command-line parameters described above.
The messages that the server sends to the client are also inputs. The client should be prepared to receive these message types:

- OK
- GRID
- GOLD
- DISPLAY
- QUIT
- ERROR

Output: Per the requirements spec, the client will print the game summary it has received from the server. 

The client will not print anything out to stdout other than what is required for game play, but will log errors to stderr that can be saved in a log file. 
The messages that the client sends to the server are also outputs. These include:

- A “PLAY” real name message (with the player’s ‘real name’)
- “KEY k” where k is the single-character keystroke typed by the user
- “SPECTATE” if the client is a spectator


### Functional decomposition into modules


 - `main`, which parses arguments and initializes other modules
 - `parseArgs`, parses and validates arguments provided through the command-line
 - `sendMode`, Send appropriate mode to server based on input
 - `checkGridSize`, check if client window is larger than server map size
 - `keyListen`, look for keyboard input to send to server
 - `infoListen`, look for messages from the server to display
 - `gameOver`, print game summary upon game completion

 
### Pseudo code for logic/algorithmic flow

```
parse the command line, validate parameters, send message to the server with playmode and name
check if size of display is big enough
	if not, wait till display is big enough
display status line and grid
while in play (not told to quit)
	send keystrokes to the server
	redisplay map when receiving updated map
	if receiving weird message from the server
		note on status line
print game summary
```


### Major data structures

The client will not be using any major data structures for its implementation.

---

## Server
### User interface

The server has only one interface; the command line arguments it receives. The arguments are below:

```
$ server map.txt [seed]
```

Where:
- map.txt is a text file, assumed to be a valid map (described in the Requirement Specs)
- seed is optional and is an integer used to produce random behavior (using the same seed will produce identical generations on subsequent runs); if not provided, a seed is generated using getpid()



### Inputs and outputs

Input: There is one input file, map.txt. A map file is a text file with exactly NR lines and in which every line has exactly NC characters. All maps given are assumed to be valid; NR and NC are inferred by reading the file.
The second argument is a command-line parameter described above. 

In an abstracted sense, the messages that the server receives from the client will also be inputs. The server should be prepared to receive these message types:

- PLAY
- SPECTATE
- KEY

Output: Per the requirements spec, the primary output of the server will occur when the game has ended. This will be:

- A game-over summary (which includes):
  - with the phrase “GAME OVER” 
  - A simple textual table with one row for every player (including any who quit early) and with three columns: (may be sorted by letter, score, or unsorted)
    - Player letter
    - Player purse
    - Player name

The server will not print anything out to stdout other than what is required for game play, but will log errors to stderr that can be saved in a log file. 

The messages that the server sends back to the client are also outputs. These include:

- A “QUIT” message if the game is already full, or if the player’s name was not provided
- An “OK” message if the player has been added to the game
- A “GRID” message that sends the number of rows and number of columns to the client
- A “GOLD” message that contains information about the number of gold nuggets collected, the number of gold nuggets in a players purse, and how many nuggets are to be found
- A “DISPLAY” string which is a multi-textual representation of the grid as it is known/seen by the client


### Functional decomposition into modules

 - `main`, which parses arguments and initializes other modules
 - `parseArgs`, parses and validates arguments provided through the command-line
 - `clientListen`, Look for keyboard inputs from the user and process accordingly
 - `createMap`, Use map file from client to generate game map for client
 - `movePlayer`, moves player on map according to keyboard input
 - `isVisible`, check overall map visibility for a player given position
 - `checkStatus`, check current status of game and update accordingly
 - `messageClient`,  Send client information about the game	
 - `quitGame`, End game and send summary to client

### Pseudo code for logic/algorithmic flow

The server will run as follows:

```
parse the command line and validate parameters
create random numbers for gold pile generation with seed if passed, otherwise random
load map file into the grid structure
update grid with “randomized” gold piles
open the server and show port number
while there is still gold
read messages from clients
if spectator joins
	if there is already a spectator
		send quit message to previous spectator
	add spectator
if player joins
	as long as are less than 26 players
		add player with the next letter of the alphabet
	otherwise send a message back saying its full
	if movement key is sent
move the player (update the grid and total gold count)
	if quit or EOF is sent
call a method to remove the player from the server and message them to quit
Send quit message to all clients with game summary to print
```


### Major data structures

- player is a struct to keep track of important stats of a player that contains the following
  - int row - the player’s row location in the grid
  - int column - the player’s column location in the grid
  - int gold - the amount of gold collected by this player
  - const char letter - the letter this player appears as on the map for other players
  - const char* name - the entered name of this player
- grid is a struct to keep track of everything to do with the game map from what objects are where to each player’s visibility and mapping. The grid is a 2d array of a struct called gridpoint that contains the following:
  - const char object - char that is rendered in this location, determined by the original map
  - char objectOnTop - char that is over a point (like another player or gold), null if none
  - const bool block - bool to keep track of if a block is a “see through” space or a block that blocks visibility (this simplifies the visibility function)
  - const bool movable - bool to keep track of if a block is a space that can be traveled on or not (this simplifies the player movement function)
  - int gold - keeps track of the amount of gold present in this location, often is 0 since most points will not have a gold pile
  - set seen - set that keeps track of if a point has been seen by a player
  - The (key, item) pair for set seen will be of (player, bool)
  - set visible - set that keeps track of if a point is visible by the player to render objectOnTop instead of object – null if object is not movable or is block since visibility would be useless on those blocks
  - The (key, item) pair for set visible will be of (player, bool)
- set is a data structure given in libcs50 that will be used within gridpoint and can be used to map a letter to the corresponding player to be able to find a player from just the letter.

---

## EXTRA CREDIT 

#### 2. Visibility
Limits player's visibility to a radius of 5.

#### 3. Gold Stealing
A feature for a player to steal another player's gold if they move into the other player. Can only steal their most recently pciked up pile of gold. Cannot steal again until more gold is picked up by the other player.

#### 4. Gold pile upon quitting
A feature that causes a player to drop all their gold as a pile of gold in the exact spot they are when they leave the game.