/*
 * playertest.c: Test program for the player module for the CS50 nuggets project
 *
 * Riley Johnson, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "mem.h"
#include "message.h"
#include "file.h"

/* ********** main() ********** */
int
main()
{
  // Initialize player
  printf("Initialize player\n");
  printf("Please initialize a server on the same server this program is being");
  printf(" run on and provide the port number\n");
  char* portNumber = file_readLine(stdin);
  addr_t address;
  message_setAddr("localhost", portNumber, &address);
  mem_free(portNumber);
  char* test_name = mem_malloc(7);
  strcpy(test_name, "tester");
  player_t* player = player_new(5, 5, 'a', test_name, address);
  if (player == NULL) {
    fprintf(stderr, "player_new failed for player\n");
    return 1;
  } else {
    printf("Success!\n\n");
  }

  // Check initialized values using getters
  printf("Check values of player; the values should be\n");
  printf("Row: 5, Column: 5, Letter: a, Name: tester, Gold: 0\n");
  int row = player_get_row(player);
  int column = player_get_column(player);
  char character = player_get_letter(player);
  char* name = player_get_name(player);
  int gold = player_get_gold(player);
  printf("Values are:\n");
  printf("Row: %d, Column: %d, Letter: %c, Name: %s, Gold: %d\n\n", row, column, 
      character, name, gold);

  // Check gold adding capabilities
  printf("Check gold adding capabilities\n");
  printf("Adding valid amount; 1, new value should be 1\n");
  player_add_gold(player, 1);
  gold = player_get_gold(player);
  printf("Gold value is now %d\n", gold);
  printf("Adding invalid amount; -1, value should still be 1\n");
  player_add_gold(player, -1);
  gold = player_get_gold(player);
  printf("Gold value is now %d\n\n", gold);

  // Check location changing capabilities
  printf("Check location changing capabilities\n");
  printf("Changing to valid location: Row 1, Column 1\n");
  player_set_location(player, 1, 1);
  row = player_get_row(player);
  column = player_get_column(player);
  printf("Location is now Row %d Column %d\n", row, column);
  printf("Changing to invalid location: Row -1, Column -1, location should ");
  printf("not change\n");
  player_set_location(player, -1, -1);
  row = player_get_row(player);
  column = player_get_column(player);
  printf("Location is now Row %d Column %d\n\n", row, column);

  // Check address changing capabilities
  printf("Check address changing capabilities\n");
  printf("Current address is");
  printf("%s\n", message_stringAddr(player_get_address(player)));
  printf("Please initialize another server on the same server this program is");
  printf(" being run on and provide the port number\n");
  portNumber = file_readLine(stdin);
  printf("Setting the address of the player to this port at localhost\n");
  addr_t address2;
  message_setAddr("localhost", portNumber, &address2);
  mem_free(portNumber);
  player_set_address(player, address2);
  printf("The address is now");
  printf("%s\n\n", message_stringAddr(player_get_address(player)));

  // Delete player
  printf("Delete player\n");
  player_delete(player);

  return 0;
}
