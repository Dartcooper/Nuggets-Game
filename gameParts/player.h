/* 
 * player.h - header file for player module
 *
 * A *player* maintains the information for a single player. This holds location
 * gold collected, representation letter, and player name. The caller 
 * can get any of these items and can add gold and set location.
 *
 * Cooper LaPorte, Febuary 2023
 */


#include <stdio.h>
#include <stdlib.h>
#include "../libcs50/mem.h"
#include "../support/message.h"


/**************** global types ****************/
typedef struct player player_t;  // opaque to users of the module

/**************** functions ****************/

/**************** player_new ****************/
/* Create a new player.
 * 
 * Caller provides:
 *   valid int, int, char, and char*
 * We return:
 *   pointer to a new player, or NULL if error.
 * Caller is responsible for:
 *   later calling player_delete.
 * Note:
 *    caller in not resposible for deleting the char* name, player_delete will do that
 */
player_t* player_new(int row, int column, char letter, char* name, addr_t addr);

/**************** player_add_gold ****************/
/* Add gold to the player
 *
 * Caller provides:
 *   valid player pointer, valid int
 * Notes:
 *   If the player is null, nothing happens
 *   If gold becomes less than 0, set gold to 0, should not happen
 */
void player_add_gold(player_t* player, const int goldAmount);

/**************** player_set_location ****************/
/* Set row and column of the player
 *
 * Caller provides:
 *   valid player pointer, int row > 0 and column > 0
 * Notes:
 *   If the player is null or the row or column is not valid, nothing happens
 * Assumption:
 *   The row and column are a valid spot for a player to be in
 */
void player_set_location(player_t* player, const int row, const int column);


/**************** player_set_address ****************/
/* Set address of the player
 *
 * Caller provides:
 *   valid player pointer, valid address struct
 * Notes:
 *   If the player is null, nothing happens
 */
void player_set_address(player_t* player, addr_t from); 


/**************** player_set_goldFound ****************/
/* Set row and column of the player
 *
 * Caller provides:
 *   valid player pointer, int gold found
 * Notes:
 *   If the player is null, nothing happens
 */
void player_set_gold_found(player_t* player, const int gold);

// EXTRA CREDIT 3 START
/**************** player_steal_gold ****************/
/* steal the most recent pile of gold collected from a player
 * 
 * Caller provides:
 *   two valid player pointer
 * Notes:
 *   If either player is null, nothing happens
 *   Otherwise removes the most recent pile size of gold from the victim and gives it to the robber
 */
void player_steal_gold(player_t* robber, player_t* victim);
// EXTRA CREDIT 3 END

/**************** player_get_gold ****************/
/* Get the amount of gold from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The int of the amount of gold the player has
 *   If the player is null, return -1
 */
int player_get_gold(player_t* player);

/**************** player_get_row ****************/
/* Get the row from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The row of the player
 *   If the player is null, return -1
 */
int player_get_row(player_t* player);

/**************** player_get_column ****************/
/* Get the column from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The column of the player
 *   If the player is null, return -1
 */
int player_get_column(player_t* player);

/**************** player_get_letter ****************/
/* Get the letter from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The letter of the player
 *   If the player is null, return '\0'
 */
char player_get_letter(player_t* player);

/**************** player_get_name ****************/
/* Get the name from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The name of the player
 *   If the player is null, return null
 */
char* player_get_name(player_t* player);

/**************** player_get_gold_found ****************/
/* Get the amount of gold player has just found.
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   The int of the amount of gold the player has just found
 *   If the player is null, return -1
 */
int player_get_gold_found(player_t* player);


/**************** player_get_address ****************/
/* Get the address from the player
 *
 * Caller provides:
 *   valid player pointer
 * We return:
 *   valid address struct 
 *   If the player is null, return null
 */
addr_t player_get_address(player_t* player);

/**************** player_delete ****************/
/* Delete player, freeing the name.
 *
 * Caller provides:
 *   valid player pointer,
 * We do:
 *   if player==NULL, do nothing.
 *   free the string for name, and the player itself.
 */
void player_delete(player_t* player);
