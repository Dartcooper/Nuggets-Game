# Client 

The client directory contains the code to build a 'client', which is able to join the game hosted by the server. The client allows a human to join the game as an interactive player or as a spectator. As many as 26 players may join the game and play at a time. A client can also join as a 'spectator', meaning that they can see all players and the entirety of the playing map, but cannot actually play or collect gold themselves. To join an exisiting game as a player, run:

```
$ ./client HostName Port 'PlayerName'
```

To join as a spectator, simply do not provide a playername argument.

### Player keystrokes: 
All keystrokes are sent immediately to the server. The valid keystrokes for the player are

Q quit the game.

h move left, if possible

l move right, if possible

j move down, if possible

k move up , if possible

y move diagonally up and left, if possible

u move diagonally up and right, if possible

b move diagonally down and left, if possible

n move diagonally down and right, if possible


