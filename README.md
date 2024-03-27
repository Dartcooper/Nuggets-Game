# Nuggets

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

## Group Members
* Arun Guruswamy
* Riley Johnson
* Cooper LaPorte 
* Jorie MacDonald

## Directory Layout
Client and server (and all necessary libraries) can be built using the top-level Makefile. They can also be built via Makefile in their own respective subdirectories.

### Server
The server directory contains the code to build the server on which the game is run. Server takes the direct path to a map file as an argument, as well as an optional integer 'seed'. 
To start up the server for a new game, run:
```
$ ./server '../maps/main.txt'
```

### Client
The client directory contains the code to build a 'client', which is able to join the game hosted by the server. As many as 26 players may join the game and play at a time. A client can also join as a 'spectator', meaning that they can see all players and the entirety of the playing map, but cannot actually play or collect gold themselves.
To join an exisiting game as a player, run:
```
$ ./client HostName Port 'PlayerName'
```
To join as a spectator, simply do not provide a playername argument. 
#### GameParts
Library that contains all the component parts of the game, including the 'grid' and 'player' modules. 

#### LibCS50
Imported library provided by CS50 Instructors at Dartmouth College, included specifically for useage of the "set" module. 

#### Support
Library provided by CS50 Instructors, with support modules such as 'message' that help with server/client functionality. 
