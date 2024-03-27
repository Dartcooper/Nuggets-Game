/*
 * gridtest.c: Test program for the grid module for the CS50 nuggets project
 *
 * Riley Johnson, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "player.h"
#include "grid.h"
#include "file.h"

/* Function prototypes: defined below main */
static void setValues(int row, int column, grid_t* grid, player_t* player, 
    char* object, char* objectOnTop, bool* isBlock, bool* isMovable, 
    bool* seen, bool* visible);
static void printValues(char object, char objectOnTop, bool isBlock,
    bool isMovable, bool seen, bool visible);
static void printBool(bool boolean);
static void testRandomness(char* mapPath, const int* seed);

/* ********** main() ********** */
int
main()
{
  // Variables
  grid_t* grid = NULL;
  player_t* player = NULL;
  char* player_name = NULL;
  FILE* mapFile = NULL;
  char* mapString = NULL;
  char* mapPath = NULL;
  char* portNumber = NULL;
  char object = '\0';
  char objectOnTop = '\0';
  bool isBlock = false;
  bool isMovable = false;
  bool seen = false;
  bool visible = false;

  // Initialize grid
  printf("Initialize grid\n");
  mapPath = "../maps/main.txt";
  grid = grid_new(mapPath, NULL);
  if (grid == NULL) {
    fprintf(stderr, "grid_new failed for grid\n");
    return 1;
  } else {
    printf("Success!\n\n");
  }

  // Initialize player
  printf("Initialize player\n");
  printf("Please initialize a server on the same server this program is being");
  printf("run on and provide the port number\n");
  portNumber = file_readLine(stdin);
  addr_t address;
  message_setAddr("localhost", portNumber, &address);
  mem_free(portNumber);
  player_name = mem_malloc(5);
  strcpy(player_name, "test");
  player = grid_add_player(grid, "A", player_name, address);
  if (player == NULL) {
    fprintf(stderr, "grid_add_player failed for grid\n");
    return 2;
  } else {
    printf("Success!\n\n");
  }

  // Test getter and is methods
  printf("Test the getter and is methods\n");
  printf("Testing on row 2, column 5; results should be\n");
  printf("Object: ., ObjectOnTop: null OR * OR a, isBlock: false,"); 
  printf(" isMovable: true, Seen: false, Visible: false\n");
  setValues(2, 5, grid, player, &object, &objectOnTop, &isBlock, &isMovable, 
      &seen, &visible);
  printf("Values are:\n");
  printValues(object, objectOnTop, isBlock, isMovable, seen, visible);

  printf("Testing on row 0, column 3; results should be\n");
  printf("Object: -, ObjectOnTop: null, isBlock: true, isMovable: false,"); 
  printf(" Seen: false, Visible: false\n");
  setValues(0, 3, grid, player, &object, &objectOnTop, &isBlock, &isMovable, 
      &seen, &visible);
  printf("Values are:\n");
  printValues(object, objectOnTop, isBlock, isMovable, seen, visible);

  printf("Testing on row 2, column 14; results should be\n");
  printf("Object: #, ObjectOnTop: null, isBlock: true, isMovable: true,");
  printf(" See: false, Visible: false\n");
  setValues(2, 14, grid, player, &object, &objectOnTop, &isBlock, &isMovable, 
      &seen, &visible);
  printf("Values are:\n");
  printValues(object, objectOnTop, isBlock, isMovable, seen, visible);

  printf("Testing on row 0, column 0; results should be\n");
  printf("Object:  , ObjectOnTop: null, isBlock: true, isMovable: false,");
  printf(" Seen: false, Visible: false\n");
  setValues(0, 0, grid, player, &object, &objectOnTop, &isBlock, &isMovable, 
      &seen, &visible);
  printf("Values are:\n");
  printValues(object, objectOnTop, isBlock, isMovable, seen, visible);
  printf("\n");

  // Test grid_set_objectOnTop
  printf("Test grid_set_objectOnTop\n");
  printf("Changing the objectOnTop of row 3, column 7 to *\n");
  grid_set_objectOnTop(grid, 3, 7, '*');
  objectOnTop = grid_get_objectOnTop(grid, 3, 7);
  printf("ObjectOnTop is now %c\n\n", objectOnTop);

  // Test grid_update_visiblity and grid_get_map
  printf("Test grid_update_visiblity and grid_get_map\n");
  printf("Update the seen/visible sets for player\n");
  grid_update_visibility(grid, player);
  printf("Print out the map according to the spectator's view\n");
  mapString = grid_get_map(grid, NULL);
  printf("%s\n", mapString);
  mem_free(mapString);
  printf("This should match (except for the presence of a player and gold");
  printf(" piles) the map printed out from the file, shown below:\n");
  mapFile = fopen(mapPath, "r");
  mapString = file_readFile(mapFile);
  fclose(mapFile);
  printf("%s\n", mapString);
  mem_free(mapString);
  printf("Print out the map according to the player's view\n");
  mapString = grid_get_map(grid, "A");
  printf("%s\n", mapString);
  mem_free(mapString);
  printf("Check that it matches up with the visibility section of the "); 
  printf("reqs spec\n\n");

  // Delete grid and player
  printf("Delete grid and player\n");
  player_delete(player);
  grid_delete(grid);

  // Test randomness (gold generation/player spawns)
  int seed = 10;
  printf("Test randomness (gold generation/player spawns)\n\n");
  testRandomness(mapPath, NULL);
  testRandomness(mapPath, &seed);
  testRandomness(mapPath, &seed);

  return 0;
}

/* ********** setValues() ********** */
/* Helper function that sets the values of chars and bools according to info
 * found at a gridpoint at a given column and row.
 */
static void
setValues(int row, int column, grid_t* grid, player_t* player, char* object, 
    char* objectOnTop, bool* isBlock, bool* isMovable, bool* seen, bool* visible) 
{
  *object = grid_get_object(grid, row, column);
  *objectOnTop = grid_get_objectOnTop(grid, row, column);
  *isBlock = grid_isBlock(grid, row, column);
  *isMovable = grid_isMovable(grid, row, column);
  *seen = grid_get_seen(grid, row, column, "A");
  *visible = grid_get_visible(grid, row, column, "A");
}

/* ********** printValues() ********** */
/* Helper function that prints out values associated with a gridpoint at a given
 * column and row.
 */
static void
printValues(char object, char objectOnTop, bool isBlock, bool isMovable,
    bool seen, bool visible)
{
  printf("Object: %c, ObjectOnTop: ", object);
  if (objectOnTop == '\0') {
    printf("null");
  } else {
    printf("%c", objectOnTop);
  }
  printf(", isBlock: ");
  printBool(isBlock);
  printf(", isMovable: ");
  printBool(isMovable);
  printf(", Seen: ");
  printBool(seen);
  printf(", Visible: ");
  printBool(visible);
  printf("\n");
}

/* ********** printBool() ********** */
/* Helper function that prints "true" if a bool is true, false otherwise. */
static void
printBool(bool boolean)
{
  if (boolean) {
    printf("true");
  } else {
    printf("false");
  }
}

/* ********** testRandomness() ********** */
/* Helper function that creates a grid, prints the gold count and location of 
 * the first gold pile it finds as well as the loction of a player it spawns in.
 */
static void
testRandomness(char* mapPath, const int* seed)
{
  // Variables
  FILE* mapFile = NULL;
  grid_t* grid = grid_new(mapPath, seed);
  printf("Please initialize another server on the same server this program is");
  printf(" being run on and provide the port number\n");
  char* portNumber = file_readLine(stdin);
  addr_t address;
  message_setAddr("localhost", portNumber, &address);
  mem_free(portNumber);
  char* player_name = mem_malloc(5);
  strcpy(player_name, "test");
  player_t* player = grid_add_player(grid, "A", player_name, address);
  int goldRow = 0;
  int goldColumn = 0;
  int playerRow = 0;
  int playerColumn = 0;
  int gold = 0;
  int goldTaken = 0;
  
  // Loop over every gridpoint until gold is found
  mapFile = fopen(mapPath, "r");
  int rows = file_numLines(mapFile);
  char* line = file_readLine(mapFile);
  int columns = strlen(line);
  mem_free(line);
  fclose(mapFile);
  for (int i = 0; i < rows; i++) {
    if (gold != 0) {
      break;
    }
    for (int j = 0; j < columns; j++) {
      if ((gold = grid_get_gold(grid, i, j)) != 0) {
        goldRow = i;
        goldColumn = j;
        break;
      }
    }
  }

  // Print gold count/location and player location
  playerRow = player_get_row(player);
  playerColumn = player_get_column(player);
  printf("%d gold found at row %d, column %d\n", gold, goldRow, goldColumn);
  printf("Player found at row %d, column %d\n\n", playerRow, playerColumn);

  // Only on the first run
  if (seed == NULL) {
    goldTaken = grid_take_gold(grid, goldRow, goldColumn);
    gold = grid_get_gold(grid, goldRow, goldColumn);
    printf("Took %d gold from row %d, column %d; the gold count there is now %d\n\n",
        goldTaken, goldRow, goldColumn, gold);
  }
  player_delete(player);
  grid_delete(grid);
}
