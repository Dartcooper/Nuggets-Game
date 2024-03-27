/* 
 * grid.c - CS50 'grid' module
 *
 * see grid.h for more information.
 *
 * Cooper LaPorte, Febuary 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "set.h"
#include "player.h"
#include "file.h"
#include "grid.h"
#include "counters.h"
#include "mem.h"
#include "../support/message.h"

/**************** game constants ****************/
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/**************** local types ****************/
typedef struct gridpoint{
int row;            // row of gridpoint
int column;         // column of gridpoint
char object;  // char that is rendered in this location, determined by the original map
char objectOnTop;   // char that is over a point (like another player or gold), null if none
bool block;   // bool to keep track of if a block is a “see through” space or a block 
// that blocks a player’s visibility (this simplifies the visibility function)
bool movable; // bool to keep track of if a block is a space that can be traveled on or not
// (this simplifies the player movement function)
int gold;           // keeps track of the amount of gold present in this location,
 // often is 0 since most points will not have a gold pile
set_t* seen;        // set that keeps track of if a point has been seen by a player,
// the (key, item) pair for set seen will be of (player, bool)
set_t* visible;     // set that keeps track of if a point is visible by the player to render 
// objectOnTop instead of object, null if object is not movable or is block since visibility 
// would be useless on those blocks, the (key, item) pair for set visible will be of (player, bool)
} gridpoint_t;

/**************** global types ****************/
typedef struct grid{
gridpoint_t*** gridArray; // 2d array of gridpoint pointers
int numOfRows;
int numOfColumns;
int totalGoldLeft;
} grid_t;

/**************** local functions ****************/
/* not visible outside this file */
static gridpoint_t* gridpoint_new(char object, int row, int column);
static bool gold_fill(grid_t* grid, const int* seed, int rows, int columns);
static bool isVisible(grid_t* grid, player_t* player, int row, int column);
static void deleteBool(void* boolean);

/**************** grid_new ****************/
/* see grid.h for description */
grid_t*
grid_new(char* mapPath, const int* seed){
    FILE* map = fopen(mapPath, "r");
    if(map == NULL){
        return NULL;
    }
    int rows = file_numLines(map); // num of rows in the file
    char* line = file_readLine(map); // a line from the file
    if(line == NULL){
        fclose(map);
        return NULL;
    }
    int columns = strlen(line); // num of columns in the file
    mem_free(line);
    fclose(map); // closed and reopened map since I had already read a line to find columns
    map = fopen(mapPath, "r");
    if (map == NULL) {
      return NULL;
    }
    grid_t* grid = mem_malloc(sizeof(grid_t));
    grid->gridArray = mem_malloc(rows*sizeof(gridpoint_t**));
    for (int i = 0; i < rows; i++) {
      grid->gridArray[i]=mem_malloc(columns*sizeof(gridpoint_t*));
    }

    if (grid == NULL) {
        fclose(map);
        return NULL;              // error allocating grid
    } else {
        grid->numOfRows = rows;
        grid->numOfColumns = columns;
        char* mapString = file_readFile(map);
        fclose(map);
        int count = 0;
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < columns; j++){
                grid->gridArray[i][j] = gridpoint_new(mapString[count], i, j);
                count++; // moves to the next char in the map string
            }
            count++; // adds one to the count at the end of each line to account for the new line character
        }
        mem_free(mapString);
        if(!gold_fill(grid, seed, rows, columns)){ // fill the gold spots
            return NULL; // error in gold fill
        }
        grid->totalGoldLeft = GoldTotal; // set total gold to start as GoldTotal
        return grid;
    }
}

/**************** gold_fill ****************/
/* fill the initial gold in a grid randomly, only meant to be called in grid_new
 * return true if successful
 */
static bool
gold_fill(grid_t* grid, const int* seed, int rows, int columns)
{
    if(seed == NULL){
        srand(getpid()); // set the random number sequence to a random random-number sequence
    } else{
        srand(*seed); // set the random-number sequence from the given seed
    }
    int numGoldPiles = (rand() % ((GoldMaxNumPiles - GoldMinNumPiles) + 1)) + GoldMinNumPiles; // random number between GoldMinNumPiles and GoldMaxNumPiles
    counters_t* distribution = counters_new(); // counters to randomly distribute the gold to the piles
    if (distribution == NULL){ // memory allocation error
        return false;
    }
    for(int i = 0; i < numGoldPiles; i++){
        counters_add(distribution, i); // adding a counter for each pile with a count of 1
    }
    for(int i = 0; i < (GoldTotal-numGoldPiles); i++){ // loop through the amount of gold left
        counters_add(distribution, (rand() % numGoldPiles)); // incrementing a random pile
    }
    set_t* eligablePoints = set_new(); // set to keep track of the points eligable for a gold pile
    if (eligablePoints == NULL){ // memory allocation error
        counters_delete(distribution); // delete the counters
        return false;
    }
    int count = 0; // how many eligable points there are
    char* numString = mem_malloc(3); // string for the set since count can't be the key
    numString[2] = '\0';
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            if(!grid->gridArray[i][j]->block){ // gridpoint can have a gold pile
                numString[0] = ((count%128) + '0');
                numString[1] = (((int)(count/128)) + '0'); // change the string to a unique trackable string (128^2 possible, should be plenty for our game)
                set_insert(eligablePoints, numString, grid->gridArray[i][j]); // add the eligable point to the set
                count++;
            }
        }
    }
    for(int i = 0; i < numGoldPiles; i++){ // placing each gold pile in a random eligable spot
        int spot = (rand() % count);
        numString[0] = ((spot%128) + '0');
        numString[1] = (((int)(spot/128)) + '0'); // change the string to a unique string for a spot
        while(((gridpoint_t*)(set_find(eligablePoints, numString)))->gold != 0){ // while the found spot already has a pile
            spot = (rand() % count); // fild a new eligable spot
            numString[0] = ((spot%128) + '0');
            numString[1] = (((int)(spot/128)) + '0');// change the string to the new unique string for spot
        }
        ((gridpoint_t*)(set_find(eligablePoints, numString)))->gold = counters_get(distribution, i); // set the gold
        ((gridpoint_t*)(set_find(eligablePoints, numString)))->objectOnTop = '*'; // set the object on top to * for gold pile
    }
    mem_free(numString);
    counters_delete(distribution); // delete the counters
    set_delete(eligablePoints, NULL); // delete the set, but not the gridpoints
    return true;
}

/**************** grid_add_player ****************/
/* see grid.h for description */
player_t*
grid_add_player(grid_t* grid, char* letter, char* name, addr_t addr)
{
    if(grid == NULL || letter == NULL || name == NULL){
        return NULL;
    }
    set_t* eligablePoints = set_new(); // set to keep track of the points eligable for a new player
    if (eligablePoints == NULL){ // memory allocation error
        return false;
    }
    int count = 0; // how many eligable points there are
    char* numString = mem_malloc(3); // string for the set since count can't be the key
    numString[2] = '\0';
    for(int i = 0; i < grid->numOfRows; i++){
        for(int j = 0; j < grid->numOfColumns; j++){
            if(!(grid->gridArray[i][j]->block) && grid->gridArray[i][j]->objectOnTop == '\0'){ // gridpoint is an open spot without a player or gold pile and not a tunnel
                numString[0] = ((count%128) + '0');
                numString[1] = (((int)(count/128)) + '0'); // change the string to a unique trackable string
                set_insert(eligablePoints, numString, grid->gridArray[i][j]); // add the eligable point to the set
                count++;
            }
        }
    }
    int spot = (rand() % count); // finding a random spot from the eligable ones
    numString[0] = ((spot%128) + '0');
    numString[1] = (((int)(spot/128)) + '0'); // change the string to a unique string for a spot
    player_t* player = player_new(((gridpoint_t*)(set_find(eligablePoints, numString)))->row, ((gridpoint_t*)(set_find(eligablePoints, numString)))->column, letter[0], name, addr); // make player
    if(player == NULL){ // error creating player
        set_delete(eligablePoints, NULL); // delete the set, but not the gridpoints
        mem_free(numString);
        return NULL;
    }
    ((gridpoint_t*)(set_find(eligablePoints, numString)))->objectOnTop = letter[0]; // set the object on top in the grid
    for(int i = 0; i < grid->numOfRows; i++){
        for(int j = 0; j < grid->numOfColumns; j++){
            grid_set_visibility(grid, i, j, letter, false); // set all visibility to start to false
            grid_set_seen(grid, i, j, letter, false); // set all seen to start to false
        }
    }
    set_delete(eligablePoints, NULL); // delete the set, but not the gridpoints
    mem_free(numString);
    return player;
}

/**************** gridpoint_new ****************/
/* Allocate and initialize a gridpoint */
static
gridpoint_t* gridpoint_new(char object, const int row, const int column)
{
  gridpoint_t* gp = mem_malloc(sizeof(gridpoint_t));
  if (gp == NULL) {
    return NULL;              // error allocating gridpoint
  } else {
    // initialize contents of gridpoint structure
    gp->row = row;
    gp->column = column;
    gp->object = object;
    gp->objectOnTop = '\0';
    if(object == '.'){
      gp->block = false;
    } else{
      gp->block = true;
    }
    if(object == '.' || object == '#') {
      gp->movable = true;
    } else{
      gp->movable = false;
    }
    gp->gold = 0;
    set_t* seen = set_new();
    if(seen == NULL) {
      return NULL;
    }
    gp->seen = seen;
    set_t* visible = set_new();
    if (visible == NULL) {
      return NULL;
    }
    gp->visible = visible;
    return gp;
  }
}

/**************** grid_update_visibility ****************/
/* see grid.h for description */
bool
grid_update_visibility(grid_t* grid, player_t* player)
{
    if(grid == NULL || player == NULL){
        return false;
    }
    char* letter = mem_malloc(2);
    letter[1] = '\0';
    letter[0] = player_get_letter(player);
    for(int i = 0; i < grid->numOfRows; i++){
        for(int j = 0; j < grid->numOfColumns; j++){
            *(bool*)set_find(grid->gridArray[i][j]->visible, letter) = isVisible(grid, player, i, j);
            // set the spot to the player's visibility
            if(*(bool*)set_find(grid->gridArray[i][j]->visible, letter)){ // if the spot is visible
                *(bool*)set_find(grid->gridArray[i][j]->seen, letter) = true; // set seen to true, should never changed again
            }
        }
    }
    mem_free(letter);
    return true;
}

/**************** isVisible ****************/
/* helper method for calculating visibility */
static bool
isVisible(grid_t* grid, player_t* player, int row, int column)
{
    if(grid == NULL || player == NULL || row < 0 || column < 0 || row >= grid->numOfRows || column >= grid->numOfColumns){
        return false;
    }
    int rp = player_get_row(player); // set rp to the player's row
    int cp = player_get_column(player); // set cp to the player's column
    // EXTRA CREDIT LIMIT VISIBILITY RANGE START
    if(( (abs(rp-row)*abs(rp-row)) + (abs(cp-column)*abs(cp-column)) ) > 25){ // if the spot is outside the range of diameter of 5 spots, cant be seen
        return false;
    }
    // EXTRA CREDIT LIMIT VISIBILITY RANGE START
    if (row == rp && column == cp) {      // same point as player; it is visible
      return true;
    } else if(row == rp && column != cp){ // there is no slope since same row
        if(cp > column){
            for(int i = column + 1; i < cp; i++){
                if(grid->gridArray[row][i]->block){ // if the block in the way is a "block"
                    return false;
                }
            }
            return true;                            // no need to go through other checks
        } else{
            for(int i = cp + 1; i < column; i++){
                if(grid->gridArray[row][i]->block){ // if the block in the way is a "block"
                    return false;
                }
            }
            return true;                            // no need to go through other checks
        }
    } else if(row != rp && column == cp){ // there is 0 slope since same column
        if(rp > row){
            for(int i = row + 1; i < rp; i++){
                if(grid->gridArray[i][column]->block){ // if the block in the way is a "block"
                    return false;
                }
            }
            return true;                               // no need to go through other checks
        } else{
            for(int i = rp + 1; i < row; i++){
                if(grid->gridArray[i][column]->block){ // if the block in the way is a "block"
                    return false;
                }
            }
            return true;                               // no need to go through other checks
        }
    }
    double slope = ((double)(column-cp))/(row-rp);
    if(row != rp && column != cp){ // make sure not dividing by 0 or is 0
        if(abs(slope) <= 1){ // loop through rows
            if(rp > row){
                for(int i = row + 1; i < rp; i++){
                    double colInt = slope * i; // calculate the column intersection for the current row
                    colInt += (column-(slope)*row); // shift by where the player is
                    if(floorf(colInt) == colInt){ // it is a whole number, only check one block
                        if(grid->gridArray[i][(int)(colInt)]->block){ // if the block in the way is a "block"
                            return false;
                        }
                    } else{ // between two columns
                        if(grid->gridArray[i][(int)(colInt)]->block && grid->gridArray[i][(int)(colInt)+1]->block){ // if both blocks in the way are a "block"
                            return false;
                        }
                    }
                }
            } else{ // have to start at rp going to row
                for(int i = rp + 1; i < row; i++){
                    double colInt = slope * i; // calculate the column intersection for the current row
                    colInt += (column-(slope)*row); // shift by where the player is
                    if(floorf(colInt) == colInt){ // it is a whole number
                        if(grid->gridArray[i][(int)(colInt)]->block){ // if the block in the way is a "block"
                            return false;
                        }
                    } else{ // between two columns
                        if(grid->gridArray[i][(int)(colInt)]->block && grid->gridArray[i][(int)(colInt)+1]->block){ // if both blocks in the way are a "block"
                            return false;
                        }
                    }
                }
            }
        } else{ // loop through columns
            if(cp > column){
                for(int i = column + 1; i < cp; i++){
                    double rowInt = (i-(column-(slope)*row))/(slope); // calculate the row intersection for the current column
                    if(floorf(rowInt) == rowInt){ // it is a whole number
                        if(grid->gridArray[(int)(rowInt)][i]->block){ // if the block in the way is a "block"
                            return false;
                        }
                    } else{ // between two rows
                        if(grid->gridArray[(int)(rowInt)][i]->block && grid->gridArray[(int)(rowInt)+1][i]->block){ // if both blocks in the way are a "block"
                            return false;
                        }
                    }
                }
            } else{
                for(int i = cp + 1; i < column; i++){
                    double rowInt = (i-(column-(slope)*row))/(slope); // calculate the row intersection for the current column
                    if(floorf(rowInt) == rowInt){ // it is a whole number
                        if(grid->gridArray[(int)(rowInt)][i]->block){ // if the block in the way is a "block"
                            return false;
                        }
                    } else{ // between two rows
                        if(grid->gridArray[(int)(rowInt)][i]->block && grid->gridArray[(int)(rowInt)+1][i]->block){ // if both blocks in the way are a "block"
                            return false;
                        }
                    }
                }
            }
        }
    } 
    return true; // if it gets through all the checks
}

/**************** grid_get_gold ****************/
/* see grid.h for description */
int
grid_get_gold(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        return grid->gridArray[row][column]->gold;
    }
    return -1;
}

// EXTRA CREDIT GOLD PILE DROP START
/**************** grid_set_gold ****************/
/* see grid.h for description */
void
grid_set_gold(grid_t* grid, const int row, const int column, int gold)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns && gold > 0){
        grid_set_objectOnTop(grid, row, column, '*'); // set object on top to gold pile object
        grid->gridArray[row][column]->gold = gold; // set gold value = to gold
    }
}

/**************** grid_set_gold_total ****************/
/* see grid.h for description */
void
grid_set_gold_total(grid_t* grid, int gold)
{
    if(grid != NULL && gold >= 0){
        grid->totalGoldLeft = gold; // set goldTotal value to gold
    }
}
// EXTRA CREDIT GOLD PILE DROP END

/**************** grid_get_total_gold ****************/
/* see grid.h for description */
int
grid_get_total_gold(grid_t* grid)
{
    if(grid != NULL){
        return grid->totalGoldLeft; // return total gold
    }
    return -1;
}

/**************** grid_get_rows ****************/
/* see grid.h for description */
int
grid_get_rows(grid_t* grid)
{
    if(grid != NULL){
        return grid->numOfRows; // return number of rows
    }
    return -1;
}

/**************** grid_get_columns ****************/
/* see grid.h for description */
int
grid_get_columns(grid_t* grid)
{
    if(grid != NULL){
        return grid->numOfColumns; // return number of columns
    }
    return -1;
}

/**************** grid_get_map ****************/
/* see grid.h for description */

char*
grid_get_map(grid_t* grid, char* player)
{
    if(grid == NULL){
        return NULL;
    }
    char* mapString = mem_malloc((grid->numOfRows) * (grid->numOfColumns) + grid->numOfRows + 1);
    // allocating size of map string, should be 1 for each gridpoint, 1 for each row (for the new line), and 1 for null character
    if(player == NULL){
        int count = 0; // easy way to keep track of where in the mapString we are
        for(int i = 0; i < grid->numOfRows; i++){
            for(int j = 0; j < grid->numOfColumns; j++){
                if(grid->gridArray[i][j]->objectOnTop != '\0'){
                    mapString[count] = grid->gridArray[i][j]->objectOnTop;
                } else{
                    mapString[count] = grid->gridArray[i][j]->object;
                }
                count++;
            }
            mapString[count] = '\n'; // next line at the end of each row
            count++;
        }
        mapString[count] = '\0'; // null terminator
        return mapString; // return spectator view
    }
    int count = 0; // easy way to keep track of where in the mapString we are
    for(int i = 0; i < grid->numOfRows; i++){
        for(int j = 0; j < grid->numOfColumns; j++){
            if(*(bool*)(set_find(grid->gridArray[i][j]->seen, player))){ // if the player has seen the spot
                if(*(bool*)(set_find(grid->gridArray[i][j]->visible, player))){ // if the player can see the spot
                    if(grid->gridArray[i][j]->objectOnTop != '\0'){ // if there is an object on top
                        if(grid->gridArray[i][j]->objectOnTop == player[0]){ // if the player is in this spot
                            mapString[count] = '@'; // set the player's spot as @ for the player
                        } else{
                            mapString[count] = grid->gridArray[i][j]->objectOnTop; // otherwise place object on top here
                        }
                    } else{ // no object on top
                        mapString[count] = grid->gridArray[i][j]->object;
                    }
                } else{ // has seen but cant see currently
                    mapString[count] = grid->gridArray[i][j]->object;
                }
            } else{ // hasnt seen it
                mapString[count] = ' '; // if unseen, print blank
            }
            count++;
        }
        mapString[count] = '\n'; // next line at the end of each row
        count++;
    }
    mapString[count] = '\0'; // null terminator
    return mapString; // return player specific view
}

/**************** grid_get_object ****************/
/* see grid.h for description */
char
grid_get_object(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        return grid->gridArray[row][column]->object;
    }
    return '\0';
}

/**************** grid_get_objectOnTop ****************/
/* see grid.h for description */
char
grid_get_objectOnTop(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        return grid->gridArray[row][column]->objectOnTop;
    }
    return '\0';
}

/**************** grid_isBlock ****************/
/* see grid.h for description */
bool
grid_isBlock(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        return grid->gridArray[row][column]->block;
    }
    return false;
}

/**************** grid_isMovable ****************/
/* see grid.h for description */
bool
grid_isMovable(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        return grid->gridArray[row][column]->movable;
    }
    return false;
}

/**************** grid_get_seen ****************/
/* see grid.h for description */
bool
grid_get_seen(grid_t* grid, const int row, const int column, char* player)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        if(set_find((grid->gridArray[row][column]->seen), player) == NULL){ // set find returns null for some reason
            return false;
        }
        return *(bool*)(set_find((grid->gridArray[row][column]->seen), player));
    }
    return false;
}

/**************** grid_get_visible ****************/
/* see grid.h for description */
bool
grid_get_visible(grid_t* grid, const int row, const int column, char* player)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        if(set_find((grid->gridArray[row][column]->visible), player) == NULL){ // set find returns null for some reason
            return false;
        }
        return *(bool*)(set_find((grid->gridArray[row][column]->visible), player));
    }
    return false;
}

/**************** grid_set_objectOnTop ****************/
/* see grid.h for description */
void
grid_set_objectOnTop(grid_t* grid, const int row, const int column, char objectOnTop)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        grid->gridArray[row][column]->objectOnTop = objectOnTop;
    }
}

/**************** grid_take_gold ****************/
/* see grid.h for description */
int
grid_take_gold(grid_t* grid, const int row, const int column)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns){
        int gold = grid->gridArray[row][column]->gold;
        grid->gridArray[row][column]->gold = 0;
        grid->totalGoldLeft -= gold; // subtract gold from the total gold counter
        return gold;
    }
    return -1;
}

/**************** grid_set_visibility ****************/
/* see grid.h for description */
bool
grid_set_visibility(grid_t* grid, const int row, const int column, char* player, bool visibility)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns && player != NULL){
        if(set_find(grid->gridArray[row][column]->visible, player) == NULL){
            bool* isVisible = mem_malloc(sizeof(bool));
            *isVisible = visibility;
            if(set_insert(grid->gridArray[row][column]->visible, player, isVisible) == false){
                return false;
            } else{
                return true;
            }
        }
        *(bool*)set_find(grid->gridArray[row][column]->visible, player) = visibility;
        return true;
    }
    return false;
}

/**************** grid_set_seen ****************/
/* see grid.h for description */
bool
grid_set_seen(grid_t* grid, const int row, const int column, char* player, bool seen)
{
    if(grid != NULL && row >= 0 && column >= 0 && row < grid->numOfRows && column < grid->numOfColumns && player != NULL){
        if(set_find(grid->gridArray[row][column]->seen, player) == NULL){
          bool* isSeeable = mem_malloc(sizeof(bool));
          *isSeeable = seen;
            if(set_insert(grid->gridArray[row][column]->seen, player, isSeeable) == false){
                return false;
            } else{
                return true;
            }
        }
        *(bool*)set_find(grid->gridArray[row][column]->seen, player) = seen;
        return true;
    }
    return false;
}

/**************** grid_delete ****************/
/* see grid.h for description */
void
grid_delete(grid_t* grid)
{
    if(grid != NULL){
        for(int i = 0; i < grid->numOfRows; i++){
            for(int j = 0; j < grid->numOfColumns; j++){
                set_delete(grid->gridArray[i][j]->seen, deleteBool);
                set_delete(grid->gridArray[i][j]->visible, deleteBool);
                mem_free(grid->gridArray[i][j]);
            }
            mem_free(grid->gridArray[i]);
        }
        mem_free(grid->gridArray);
        mem_free(grid);
    }
}

/**************** deleteBool ****************/
/* helper function for set_delete that frees bool in set */
void
deleteBool(void* boolean)
{
  mem_free(boolean);
}
