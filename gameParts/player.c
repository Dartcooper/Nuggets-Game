/* 
 * player.c - CS50 'player' module
 *
 * see player.h for more information.
 *
 * Cooper LaPorte, Febuary 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "player.h"
#include "../libcs50/mem.h"
#include "../support/message.h"



/**************** global types ****************/
typedef struct player{
int row; // the player’s row location in the grid
int column; // the player’s column location in the grid
int gold; // the amount of gold collected by this player, starts 0
char letter; // the letter this player appears as on the map for other players
char* name; // the entered name of this player
int goldFound;
addr_t address; // Address of player
} player_t;


/**************** player_new() ****************/
/* see player.h for description */
player_t*
player_new(int row, int column, char letter, char* name, addr_t addr)
{
  player_t* player = mem_malloc(sizeof(player_t));
  if (player == NULL) {
    return NULL;              // error allocating player
  } else {
    // initialize contents of player structure
    player->row = row;
    player->column = column;
    player->letter = letter;
    player->name = mem_malloc(51);
    strcpy(player->name, name);
    player->gold = 0;     // all player gold starts at 0
    player->goldFound = 0;
    player->address = addr;
    return player;
  }
}

/**************** player_set_address ****************/
/* see player.h for description */
void player_set_address(player_t* player, addr_t from) {
  if (message_isAddr(from)) {
    player->address = from;
  }
}

/**************** player_add_gold ****************/
/* see player.h for description */
void
player_add_gold(player_t* player, const int goldAmount)
{
  if(player != NULL){
    player->gold += goldAmount;
    if(player->gold < 0){
      player->gold = 0;
    }
  }
}

/**************** player_set_location ****************/
/* see player.h for description */
void
player_set_location(player_t* player, const int row, const int column)
{
  if(player != NULL && row > 0 && column > 0){
    player->row = row;
    player->column = column;
  }
}

/**************** player_set_gold_found ****************/
/* see player.h for description */
void
player_set_gold_found(player_t* player, const int gold)
{
  if(player != NULL){
    player->goldFound = gold;
  }
}


/**************** player_get_gold ****************/
/* see player.h for description */
int
player_get_gold(player_t* player)
{
  if(player != NULL){
    return player->gold;
  }
  return -1;
}

/**************** player_get_address ****************/
/* see player.h for description */
addr_t 
player_get_address(player_t* player) {
  if (player != NULL) {
    return player->address;
  }
  return message_noAddr();
}

/**************** player_get_row ****************/
/* see player.h for description */
int
player_get_row(player_t* player)
{
  if(player != NULL){
    return player->row;
  }
  return -1;
}

/**************** player_get_column ****************/
/* see player.h for description */
int
player_get_column(player_t* player)
{
  if(player != NULL){
    return player->column;
  }
  return -1;
}

/**************** player_get_letter ****************/
/* see player.h for description */
char
player_get_letter(player_t* player)
{
  if(player != NULL){
    return player->letter;
  }
  return '\0';
}

/**************** player_get_name ****************/
/* see player.h for description */
char*
player_get_name(player_t* player)
{
  if(player != NULL){
    return player->name;
  }
  return NULL;
}

// EXTRA CREDIT 3 START
/**************** player_steal_gold ****************/
/* see player.h for description */
void
player_steal_gold(player_t* robber, player_t* victim)
{
  if(robber != NULL && victim != NULL && player_get_gold_found(victim)>0){
    player_add_gold(robber, player_get_gold_found(victim)); // give the robber the most recent pile's worth of gold
    player_add_gold(victim, -player_get_gold_found(victim)); // remove the gold stolen
    player_set_gold_found(robber, player_get_gold_found(victim)); // robber's most recent pile is the one stolen
    player_set_gold_found(victim, 0); // youve been mugged
  }
}
// EXTRA CREDIT 3 END

/**************** player_get_gold_found ****************/
/* see player.h for description */
int
player_get_gold_found(player_t* player)
{
  if(player != NULL){
    return player->goldFound;
  }
  return -1;
}

/**************** player_delete ****************/
/* see player.h for description */
void
player_delete(player_t* player)
{
  if(player != NULL){
    mem_free(player->name);
    mem_free(player);
  }
}
