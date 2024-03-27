# CS50 Nuggets
## Implementation Spec
### Breaking Bash, Winter, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also include the grid and player modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

#### Arun:
Design and implement client module

#### Jorie:
Project manager
Client Server interfacing

#### Cooper:
Write core game logic that includes player visibility, player movement in map, and map architecture

#### Riley:
Test client and server modules through various forms (Integration, System, Unit)


## Player


### Definition of function prototypes

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[]);
static void checkWindowSize(char* message);
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function to parse and validate the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function to check if the client window is bigger than game map size, and pause if it is not.

```c
static int checkWindowSize(char* message);
```

A function to handle any messages received from the server. Exact behavior depends on the type of message received.

```c
static int handleMessage(void* arg, const addr_t from, const char* message);
```

A function to handle any keyboard input received from the client into stdin.

```c
static int handleInput(void* arg);
```

### Detailed pseudo code

#### `parseArgs`:

	Validate argument count
	Validate port number
	Initialize Message module
	
#### `checkWindowSize`:

	Parse map width and height from message
	Get width and height of client window
Loop until client window dimensions are larger than map dimensions
First iteration of loop, print message if window is smaller	

#### `handleMessage`:

	If OK message
		Print player successfully added
	If GRID message
		Call checkWindowSize
		Print statement that window is big enough
	If DISPLAY message
		Print message
	If QUIT message
		Print message
		Return true

	Return false

#### `handleInput`:
	
	Check if server address is NULL
		Return true
	Check if server address is invalid
		Return true
	
	Read a line from stdin into char buffer 
	
	If line is NULL or q
		Send KEY q message to server
		Return false
	Else 
		Strip trailing newline
		Send line to server
		Return false
	
---

## Server

### Data structures

We use four data structures:
* The `player` struct described in the design specification to keep track of player attributes, described more in the `player` module.
* The `gridpoint` struct, described in design spec to keep track of all the information about that specific gridpoint, described more in the `grid` module.
* The `grid` structure described in the design specification will be a two dimensional array of the gridpoint struct, described more in the `grid` module.
* A `set` of (char, player) where char is the letter of a player will make it easy to look up a player by their letter.
* Each `player` is initialized when a player joins the game as a player (and not a spectator).
* The `grid` is initialized when the initial map is read.
* The `gridpoints` are also initialized when the grid is, then the gold piles are later filled. Variables that are not constant can be filled and changed as gameplay progresses.
* The `set` of players starts empty and will be filled whenever a player joins a game.


### Definition of function prototypes

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `server.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** map.txt, int* seed);
static bool clientListen(void* arg, const addr_t from, char* message);
static void movePlayer(player_t* player, int columnShift, int rowShift);
static void checkStatus(game_t* game);
static void quitGame(set_t* players);
```

A function to parse and validate arguments provided through the command-line

```c
static void parseArgs(const int argc, char* argv[],
                      char** map.txt, int* seed);;
```

Look for keyboard inputs from the user and process accordingly

```c
static bool clientListen(void* arg, const addr_t from, char* message);
```

Moves player on map according to keyboard input

```c
static void movePlayer(player_t* player, int columnShift, int rowShift);
```

Check current status of game and update accordingly

```c
static void checkStatus(game_t* game);
```

A function to parse and validate arguments provided through the command-line

```c
static void parseArgs(const int argc, char* argv[],
                      char** map.txt, int* seed);;
```

End game and send summary to client

```c
static void quitGame(set_t* players);
```


### Detailed pseudo code

#### `main`:
    parse args
    createMap → store in variable
    check if createMap is NULL/unsuccessful 
        if unsuccessful, return error
    else continue

    take map.txt as input 
    process map file to infer NR lines and NC characters
    send those values to client (and client will validate)
    //if receive message from client that it’s good to go:
    check if two arguments
        if given seed:
            Check if seed is a valid int*

    else 
        set behavior to random 

    while game is not over
        if message is PLAY
            FILL IN
        if message is SPECTATE
            DO THIS
        if message is KEY
            handle movement accordingly
            if key is “k”
                move one direction
            if key is “j”
                handle movement


#### `movePlayer`:

    check if spot to move to is movable
    if not return false
    if objectOnTop is null
        move player to that spot
        return true
    if there is gold in the spot
        collect it
        move player to that spot
        return true
    if another player is there
        swap positions with the other player
    return true



## player module

### Data structures

The ‘player’ struct described in the design specification to keep track of player attributes.
```c
typedef struct player{
int row; // the player’s row location in the grid
int column; // the player’s column location in the grid
int gold; // the amount of gold collected by this player, starts 0
const char letter; // the letter this player appears as on the map for other players
const char* name; // the entered name of this player
} player_t;
```

### Definition of function prototypes

```c
player_t* player_new(int row, int column, char letter, char* name);
void player_add_gold(player_t* player, const int goldAmount);
void player_set_location(player_t* player, const int row, const int column);
int player_get_gold(player_t* player);
int player_get_row(player_t* player);
int player_get_column(player_t* player);
char player_get_letter(player_t* player);
char* player_get_name(player_t* player);
void player_delete(player_t* player);
void player_steal_gold(player_t* robber, player_t* victim);
```

A function to create a new player

```c
player_t* player_new(int row, int column, char letter, char* name);
```

A function to add gold to a player

```c
void player_add_gold(player_t* player, const int goldAmount);
```

A function to set the location of the player

```c
void player_set_location(player_t* player, const int row, const int column);
```

A function to get the amount of gold the player has collected

```c
int player_get_gold(player_t* player);
```

A function to get the row the player is in

```c
int player_get_row(player_t* player);
```

A function to get the column the player is in

```c
int player_get_column(player_t* player);
```

A function to get the letter of the player

```c
char player_get_letter(player_t* player);
```

A function to get the name of the player

```c
char* player_get_name(player_t* player);
```

A function to delete the player and its data

```c
void player_delete(player_t* player);
```

A function to let one player attain another player's gold if encountered (EXTRA CREDIT)

```c
void player_steal_gold(player_t* robber, player_t* victim);
```
—

## Grid module

### Data structures

In the grid we have 2 data structures, a grid that holds a map of gridpoints with their locations, and gridpoint that holds all the information.

The `gridpoint` struct, described in design spec to keep track of all the information about that specific gridpoint.

```c
typedef struct gridpoint{
char object; // char that is rendered in this location, determined by the original map
char objectOnTop; // char that is over a point (like another player or gold), null if none
bool block; // bool to keep track of if a block is a “see through” space or a block that blocks a player’s visibility (this simplifies the visibility function)
bool movable; // bool to keep track of if a block is a space that can be traveled on or not (this simplifies the player movement function)
int gold; // keeps track of the amount of gold present in this location, often is 0 since most points will not have a gold pile
set_t* seen; // set that keeps track of if a point has been seen by a player, the (key, item) pair for set seen will be of (player, bool)
set_t* visible; // set that keeps track of if a point is visible by the player to render objectOnTop instead of object, null if object is block since visibility would be useless on those blocks, the (key, item) pair for set visible will be of (player, bool)
} gridpoint_t;
```

The ‘grid’ structure described in the design specification will be a two dimensional array of the gridpoint struct.
```c
typedef struct grid{
gridpoint_t*** grid; // 2d array of gridpoint pointers
int numOfRows;
int numOfColumns;
int totalGoldLeft;
} grid_t;
```

### Definition of function prototypes

```c
grid_t* grid_new(char* mapPath, const int* seed);
player_t* grid_add_player(grid_t* grid, char* letter, char* name);
bool grid_update_visibility(grid_t* grid, char* player);
int grid_get_gold(grid_t* grid, const int row, const int column);
int grid_get_total_gold(grid_t* grid);
char* grid_get_map(grid_t* grid, char* player);
char grid_get_object(grid_t* grid, const int row, const int column);
char grid_get_objectOnTop(grid_t* grid, const int row, const int column);
bool grid_isBlock(grid_t* grid, const int row, const int column);
bool grid_isMovable(grid_t* grid, const int row, const int column);
bool grid_get_seen(grid_t* grid, const int row, const int column, char* player);
bool grid_get_visible(grid_t* grid, const int row, const int column, char* player);
void grid_set_objectOnTop(grid_t* grid, const int row, const int column, char objectOnTop);
int grid_take_gold(grid_t* grid, const int row, const int column);
bool grid_set_visibility(grid_t* grid, const int row, const int column, char* player, bool visibility);
bool grid_set_seen(grid_t* grid, const int row, const int column, char* player, bool seen);
void grid_delete(grid_t* grid);
void grid_set_gold(grid_t* grid, const int row, const int column, int gold);
void grid_set_gold_total(grid_t* grid, int gold);

```

A function to create the grid from a map file and possible seed, and to fill it with fandom gold piles

```c
grid_t* grid_new(char* mapPath, const int* seed);
```

A function to create a new player within a grid that finds a random eligable spot for the player

```c
player_t* grid_add_player(grid_t* grid, char* letter, char* name);
```

A function to updates a player's visibility

```c
bool grid_update_visibility(grid_t* grid, char* player);
```

A function to get the amount of gold in a spot

```c
int grid_get_gold(grid_t* grid, const int row, const int column);
```

A function to get the total gold left in the grid

```c
int grid_get_total_gold(grid_t* grid);
```

A function to get the map string to render for a given player or if no player, the spectator view

```c
char* grid_get_map(grid_t* grid, char* player);
```

A function to get the object in a gridpoint

```c
char grid_get_object(grid_t* grid, const int row, const int column);
```

A function to get the objectOnTop in a gridpoint

```c
char grid_get_objectOnTop(grid_t* grid, const int row, const int column);
```

A function to get the bool value of block

```c
bool grid_isBlock(grid_t* grid, const int row, const int column);
```

A function to get the bool value of movable

```c
bool grid_isMovable(grid_t* grid, const int row, const int column);
```

A function to get the bool value of seen for a given gridpoint and player

```c
bool grid_get_seen(grid_t* grid, const int row, const int column, char* player);
```

A function to get the bool value of visible for a given gridpoint and player

```c
bool grid_get_visible(grid_t* grid, const int row, const int column, char* player);
```

A function to set the objectOnTop for a given gridpoint

```c
void grid_set_objectOnTop(grid_t* grid, const int row, const int column, char objectOnTop);
```

A function to collect the gold from a given gridpoint

```c
int grid_take_gold(grid_t* grid, const int row, const int column);
```

A function to set the visibility bool for a given gridpoint given a player

```c
bool grid_set_visibility(grid_t* grid, const int row, const int column, char* player, bool visibility);
```

A function to set the seen bool for a given gridpoint given a player

```c
bool grid_set_seen(grid_t* grid, const int row, const int column, char* player, bool seen);
```

A function to delete the grid and all of its data

```c
void grid_delete(grid_t* grid);
```

A function to set a gold pile at a spot (EXTRA CREDIT)

```c
void grid_set_gold(grid_t* grid, const int row, const int column, int gold);
```

A function to update the total gold left in the map (EXTRA CREDIT)

```c
void grid_set_gold_total(grid_t* grid, int gold);
```

### Detailed pseudo code

#### `grid_new`:
	initialize grid
	fill all the gridpoints with information based on the passed map
	create the randomized gold piles and place them in random open spots in the map

#### `grid_add_player`:
	loop through the grid finding all the eligible spots for a player (open and no objectOnTop)
	choose one at random
	create a new player in the spot randomly chosen
	change objectOnTop to the player’s letter
	loop through each spot in the graph adding the player to the seen and visible sets as false

#### `grid_update_visibility`:
	loop through each gridpoint
		figure out if looping through rows or columns based on slope between player and point
		loop through rows/columns between the point and player
			find the intercept with the row or column (opposite of above)
			if whole number intersection
				if that gridpoint is a blocking block
	set visibility for the player to false for that gridpoint
			else
				if both gridpoints around the intersection are blocks
	set visibility for the player to false for that gridpoint
		if it gets here without setting to false, set visibility for the player to true for that gridpoint
		if the visibility for the player is true
			set the seen for the player to true

#### `grid_get_map`:
	if player is null
	loop through each row
	loop through each column
		fill this spot in the string with the object or objectOnTop in the gridpoint
	add new line character
	return the spectator map
	loop through each row
		loop through each column
	if seen for the player is true
		if visible is true for the player
			fill string spot with objectOnTop or object if none on top
			if player is the objectOnTop
				fill string spot with @
		else
			fill string spot with object
	else
		fill spot with blank space
		add new line character
	return player specific map

---


## Testing plan

A series of test maps has been provided in the maps directory.

### unit testing

#### Player
* Create a c file that does the following, run it with valgrind:
* Create a player, ensure the pointer to it isn’t null; use getter methods to check values initialized properly.
* Add gold to the player; first use a valid amount (int greater than 0) with a valid player, and then an invalid amount (less than 1)/an invalid player, use getter methods to ensure proper changes occurred.
* Change location of player; first use a valid location (column/row greater than 0), and then an invalid location, use getter methods to ensure proper changes occurred.
* Delete player; if deletion went smoothly, valgrind should report no memory leaks.

#### Grid
* Create a c file that does the following, run it with valgrind:
* Create a grid using main.txt (can be with a pointer to an actual seed or NULL), ensure the pointer to it isn’t null. Create a player using the provided grid method as well and check that their pointer isn’t null.
* Test getter and is methods on a few spots on the grid; remember, any point not containing the object ‘.’ is a block, any point containing either the object ‘.’ or ‘#’ is. movable, and any attempt to get the visibility or seen value of a gridpoint for any player should give false at this time.
* Use the setter method for objectOnTop on a gridpoint; check that the appropriate change went through using the getter method.
* Use grid_update_visiblity on the earlier created player. Then test grid_get_map for both the spectator and player view and print them. For the spectator view, check it by printing the map file out to stdout and comparing. For the player view, check that the part of the map that is visible to the player lines up with the requirements outlined in specs. 
* Delete this grid and player; if deletion went smoothly, valgrind should report no memory leaks.
* Test random gold generation and player spawns by doing the following:
* Create a grid with the seed as NULL; loop through every gridpoint of the grid with grid_get_gold until a gridpoint with gold greater than 0 is found, then print out the location of this gold and the gold count. Do grid_take_gold on that point, checking that it returns the gold count that was there and using grid_get_gold to ensure that the gold was removed. 
* Next, add a player, then print out the location of this player. Then, delete the grid.
* Repeat the above process twice, this time using a pre-chosen seed twice in a row. This time around, the printed gold locations/spawn locations should be identical to each other, but different from the previously randomly generated run. Youneed not test grid_take_gold this time around. 

### integration testing

Run both client and server with incorrect command-line arguments to ensure that parsing works correctly.

#### Client

* Run the client on the server provided by shared on a moderate-sized map (most likely main.txt) with valgrind. Ensure the following functionality:
* Make sure all inputs go through and that your character responds normally (test all controls, including diagonals and uppercase variants). 
* Make sure gold can be collected as normal and that collecting all gold in a map results in the game ending with a proper summary. 
* Ensure both Q and EOF result in the game ending as normal. 

#### Server

* Run the client provided by shared on server on a moderate-sized map (most likely main.txt) with valgrind. Ensure the following functionality:
* Attempt to join the server with a client using a blank name (a name consisting of only spaces). The client should be rejected with a message stating that a player’s name must be provided.
* Run a second client and connect it to the server and find the location of the character controlled by the first client. Attempt to walk onto the space of the first client; the two characters should swap positions. 
* Ensure that collected gold piles disappear when they are walked over (for both the client who picks it up and another client).
* When a player picks up gold, they should receive a message notifying them.
* Run it twice with a pre-chosen seed, the same one each time; the locations of gold piles, gold counts at each pile, and the spawn locations should be identical. Run it twice with a randomized seed; the locations of gold piles, gold counts at each pile, and the spawn locations should be different. These four runs need not be done with valgrind.
* Make sure walls stop motion.
* Ensure the sides of passages can’t be walked through, and that the end of a dead-end passage does not lead into empty space.
* Ensure visibility functions as normal; clients can only see rooms and passages on their screen that they are in or have already been to, and clients can only see the contents of areas they currently have line of sight with, passages are “dark” and only one space ahead at a time can be seen.

### system testing

The system testing will be testing the functionalities already proven in the integration tests. 

---

## EXTRA CREDIT 

#### 2. Visibility
Limits player's visibility to a radius of 5. Feature was added in `isVisible` in grid.c. Can be commented out to have normal visibility

#### 3. Gold Stealing
Added feature for a player to steal another player's gold if they move into the other player. Can only steal their most recently pciked up pile of gold. Cannot steal again until more gold is picked up by the other player. Feature was added in `movePlayer` in server.c. The `player_steal_gold` function was added to player.c. 

#### 4. Gold pile upon quitting
Added feature that causes a player to drop all their gold as a pile of gold in the exact spot they are when they leave the game. The feature was added in `handleMessage` in server.c. `grid_set_gold` and `grid_set_gold_total` were added to grid.c. 





