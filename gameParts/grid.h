/* 
 * graph.h - header file for graph module
 *
 * A *graph* maintains the information for the overall graph. This 2 dimensional array
 * holds the every *gridpoint* in the map which contains all the information at that point.
 * These include the object of the map, any objects on top of the map, whether the block
 * is movable or blocks visibility, and two sets to keep track of if a player 
 * has seen and can see the spot.
 * The caller can get any of these items and is able to set gold, objectOnTop, and
 * can update the sets for visibility and seen
 *
 * Cooper LaPorte, Febuary 2023
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "../libcs50/set.h"
#include "player.h"
#include "../libcs50/file.h"
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"
#include "../support/message.h"


/**************** global types ****************/
typedef struct grid grid_t;  // opaque to users of the module
typedef struct gridpoint gridpoint_t;

/**************** functions ****************/

/**************** grid_new ****************/
/* Create a new grid.
 * 
 * Caller provides:
 *   valid map file path and potentially a seed
 * 
 * We return:
 *   pointer to a new grid with filled in gridpoints from the passed map file, or NULL if error.
 * Note:
 *   a seed can be passed if we want the randomization to be a specific randomization
 *   if none is passed, a random randomization will be made
 * Caller is responsible for:
 *   later calling grid_delete.
 */
grid_t* grid_new(char* mapPath, const int* seed);

/**************** grid_add_player ****************/
/* Create a new player adding it to the grid.
 * 
 * Caller provides:
 *   valid grid and string for a player and their name
 * 
 * We return:
 *   pointer to a new player with filled in information, or NULL if error or any arguments are null.
 * Note:
 *   this will make a player and add it to a valid location in the grid, returning the player pointer
 * Caller is responsible for:
 *   checking the visibility of the player, starts as all false
 *   later calling player_delete.
 */
player_t* grid_add_player(grid_t* grid, char* letter, char* name, addr_t addr);

/**************** grid_update_visibility ****************/
/* Calculate a player's visibility and update seen and visible for each gridpoint
 * 
 * Caller provides:
 *   valid grid and player pointer
 * 
 * We return:
 *   true if seen and visible were updated in each gridpoint for the player
 *   false if unsuccessful, null grid or player, or any other error
 */
bool grid_update_visibility(grid_t* grid, player_t* player);

/**************** grid_get_gold ****************/
/* Get the amount of gold from the gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We return:
 *   The int of the amount of gold in the location
 *   If the grid is null or row or column are out of range, return -1
 */
int grid_get_gold(grid_t* grid, const int row, const int column);

// EXTRA CREDIT GOLD PILE DROP START
/**************** grid_set_gold ****************/
/* Set the amount of gold for a gridpoint for a new gold pile
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid and positive int for gold
 * Note:
 *   makes a new gold pile in the location
 */
void grid_set_gold(grid_t* grid, const int row, const int column, int gold);

/**************** grid_set_gold_total ****************/
/* Set the total gold left
 *
 * Caller provides:
 *   valid grid pointer, non-negitive int for gold
 * Note:
 *   sets the total gold left to gold
 */
void grid_set_gold_total(grid_t* grid, int gold);
// EXTRA CREDIT GOLD PILE DROP END

/**************** grid_get_total_gold ****************/
/* Get the amount of gold left in the grid
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   The int of the amount of gold total left in the grid
 *   If the grid is null, return -1
 */
int grid_get_total_gold(grid_t* grid);

/**************** grid_get_rows ****************/
/* Get the number of rows in the grid
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   The int of the number of rows in the grid
 *   If the grid is null, return -1
 */
int grid_get_rows(grid_t* grid);

/**************** grid_get_columns ****************/
/* Get the number of columns in the grid
 *
 * Caller provides:
 *   valid grid pointer
 * We return:
 *   The int of the number of columns in the grid
 *   If the grid is null, return -1
 */
int grid_get_columns(grid_t* grid);

/**************** grid_get_map ****************/
/* Get the map to print for a specific player or overall
 *
 * Caller provides:
 *   valid grid pointer, valid string for player's letter or null for player
 * We return:
 *   If player is not null, return the string that represents the map for the player with the passed letter
 *   If player is null, return the string that represents the map for the spectator
 *   If the grid is null or an error occurs, return null
 * Note:
 *   Caller is responsible for freeing the returned string
 * Assume:
 *   The char* for player is a letter corresponding with an existing player
*/
char* grid_get_map(grid_t* grid, char* player);

/**************** grid_get_object ****************/
/* Get the object from the gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We return:
 *   The char of the object in the location
 *   If the grid is null or row or column are out of range, return '\0'
 */
char grid_get_object(grid_t* grid, const int row, const int column);

/**************** grid_get_objectOnTop ****************/
/* Get the objectOnTop from the gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We return:
 *   The char of the objectOnTop in the location, will often be '\0'
 *   If the grid is null or row or column are out of range, return '\0'
 * Note:
 *   Although returning '\0' can be valid or an error
 *   the error is not fatal and is treated the same as an empty objectOnTop
 */
char grid_get_objectOnTop(grid_t* grid, const int row, const int column);

/**************** grid_isBlock ****************/
/* Check if the gridpoint is a block (blocks visibility of a player)
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We return:
 *   True if it is a block
 *   False otherwise
 * Assumption:
 *   The grid and row and column will be valid inputs
 *   If the grid is null or the row and column are out of bounds, false is still returned
 */
bool grid_isBlock(grid_t* grid, const int row, const int column);

/**************** grid_isMovable ****************/
/* Check if the gridpoint is movable (is able to be stood on by a player)
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We return:
 *   True if it is movable
 *   False otherwise
 * Assumption:
 *   The grid and row and column will be valid inputs
 *   If the grid is null or the row and column are out of bounds, false is still returned
 */
bool grid_isMovable(grid_t* grid, const int row, const int column);

/**************** grid_get_seen ****************/
/* Get the seen boolean from the gridpoint for a player
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid, and valid string for player's letter
 * We return:
 *   The value of the player in the seen set of the gridpoint, aka if the player has seen the point
 *   If the grid or player string is null or row or column are out of bounds, return false
 * Note:
 *   Although returning false can be valid or an error
 *   the error is not fatal and is treated the same as a false value
 * Assume:
 *   The char* for player is a letter corresponding with an existing player
 */
bool grid_get_seen(grid_t* grid, const int row, const int column, char* player);

/**************** grid_get_visible ****************/
/* Get the visible boolean from the gridpoint for a player
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid, and valid string for player's letter
 * We return:
 *   The value of the player in the visible set of the gridpoint, aka if the player can see the point
 *   If the grid or player string is null or row or column are out of bounds, return false
 * Note:
 *   Although returning false can be valid or an error
 *   the error is not fatal and is treated the same as a false value
 * Assume:
 *   The char* for player is a letter corresponding with an existing player
 */
bool grid_get_visible(grid_t* grid, const int row, const int column, char* player);


/**************** grid_set_objectOnTop ****************/
/* Set objectOnTop for the gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * Notes:
 *   If the grid is null or the row or column is not valid, nothing happens
 * Assumption:
 *   The row and column are valid and the objectOnTop is a valid game character that can be on top
 */
void grid_set_objectOnTop(grid_t* grid, const int row, const int column, char objectOnTop);

/**************** grid_take_gold ****************/
/* remove the gold from the gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid
 * We do:
 *   Set gold to 0 in the gridpoint if successful
 * We return:
 *   The int amount of gold from the gridpoint
 *   If the grid is null or the row or column is not valid, nothing happens and returns -1
 * Assumption:
 *   The row and column are valid
 */
int grid_take_gold(grid_t* grid, const int row, const int column);

/**************** grid_set_visibility ****************/
/* set the visibility for a player in a gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid, and valid string for player's letter
 * We do:
 *   Set player's visibility to the passed value in the gridpoint if successful
 * We return:
 *   True if successful
 *   False otherwise
 * Assume:
 *   The char* for player is a letter corresponding with an existing player
 */
bool grid_set_visibility(grid_t* grid, const int row, const int column, char* player, bool visibility);

/**************** grid_set_seen ****************/
/* set the seen for a player in a gridpoint
 *
 * Caller provides:
 *   valid grid pointer, row and column within the range of the grid, and valid string for player's letter
 * We do:
 *   Set player's seen to the passed value in the gridpoint if successful
 * We return:
 *   True if successful
 *   False otherwise
 * Assume:
 *   The char* for player is a letter corresponding with an existing player
 */
bool grid_set_seen(grid_t* grid, const int row, const int column, char* player, bool seen);

/**************** grid_delete ****************/
/* Delete grid, freeing all the gridpoints and their contents.
 *
 * Caller provides:
 *   valid grid pointer,
 * We do:
 *   if grid==NULL, do nothing.
 *   free the sets for seen and visible in each gridpoint, free the gridpoints, then the grid itself.
 */
void grid_delete(grid_t* grid);
