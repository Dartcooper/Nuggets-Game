/*
 * Server.c 
 * Jorie MacDonald, Cooper LaPorte, Riley Johnson, Arun Guruswamy, Dartmouth 23W, CS50
 */

// 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../support/message.h"
#include "../support/log.h"
#include <curses.h>
#include "../support/message.h"
#include "../support/log.h"
#include <curses.h>
#include "../gameParts/grid.h"
#include "../gameParts/player.h"
#include "../libcs50/set.h"
#include "../libcs50/mem.h"
#include <math.h>
#include <ctype.h>

static void parse_args(char* argv[], int argc); 
static bool handle_message(void* arg, const addr_t from, const char* message);
static bool movePlayer(player_t* player, grid_t* grid, int rowShift, int colShift);
static void sendMessage(player_t* player);
static void sendAll(void* arg, const char* key, void* item);
static void gameOver(); 
static void result_print(void* arg, const char* key, void* item);
static void game_over_send(void* arg, const char* key, void* item);
static void sendSpectateMessage();

typedef struct game{
    grid_t* grid;
    set_t* players; // set of the players, indexed by alphabetical letter
    set_t* player_addresses;  // set of players indexed by addresses
    bool playersFull; // boolean storing if game is full
    addr_t spectator;
} game_t;


// initalize new game
game_t* game;


// initalize player int to zero
int playerNum = 65;

int main(int argc, char* argv[]) {

    // check all the arguments
    parse_args(argv, argc);

    // unpack command line args 
    if ((argc == 2) | (argc == 3)) {

        // read in map.txt as text file
        // QUESTION: should give absolute path to map or only give it here???
        char* mapFilename = argv[1];

        // check if file is readable
        FILE* indexFile = fopen(mapFilename, "r");
        if (!indexFile) {
            fprintf(stderr, "ERROR: file is not readable. \n");
            fclose(indexFile);
            exit(1);
        }
        fclose(indexFile);
        
         
        // if seed given, use it
        int* seed;

        if (argc == 3){
            seed = mem_malloc(sizeof(int));
            *seed = atoi(argv[2]);

        }
        else {
            seed = NULL;
        }


        // initalize grid
        grid_t* grid = grid_new(mapFilename, seed);
        if (seed != NULL){
             free(seed);
        }

        // create game
        game = malloc(sizeof(game_t));
        game->playersFull = false;
        game->grid = grid;
        game->players = set_new();
        game->player_addresses = set_new();
        game->spectator = message_noAddr();
    

        // initialize the message module (without logging)
        int port = 0;
        if ((port = message_init(NULL)) == 0) {
            fprintf(stderr, "Failure to initalize message module.\n");
            exit(2); // failure to initialize message module
        }
        else {
            fprintf(stdout, "START OF LOG\n");
            fprintf(stdout, "message_init: ready at port '%i'\n", port);
            fprintf(stdout, "Ready to play, waiting at port %i\n", port);
        }
        
        

        // //Read messages from server
        message_loop(NULL, 0, NULL, NULL, handle_message);

        // message_done();

    }

    else{
        fprintf(stderr, "Inproper amount of arguments provided\n");
        exit(3);
    }
}

/******************** HELPER FUNCTIONS *****************************/

/************************** parse_args *****************************/
/* 
*/
// parses and validates arguments provided through the command-line
static void parse_args(char* argv[], int argc) {

    // ASSUME that all text files given exist and are valid maps
    // check if seed is a positive number
    if (argc == 3){
        const int seed = atoi(argv[2]);
        if (seed <= 0) {
            fprintf(stderr, "Bad seed \'%s\' (must be positive integer). \n", argv[2]);
            exit(3);
        }
    }


}

/************************** handle_message *****************************/
// PLAY 

    // if game is full or if player's name not provided, send QUIT message back
    // if player has been added to the game send OK

// SPECTATE
// KEY

static bool handle_message(void* arg, const addr_t from, const char* message) {
    char header[150];
    char content[150];
    char* grid_msg = mem_malloc(50);
    const char* msg_str = message_stringAddr(from);
    
    player_t* player;
    //player_find(from); // Need to define function  // iterate through to create fucnciton

    // break into message header and message content
    sscanf(message, "%s %s", header, content);
    
    // handle PLAY message
    if (strcmp(header, "PLAY") == 0) { 

        
        if (content == NULL){
            message_send(from, "QUIT player name not provided.");
            mem_free(grid_msg);
            return false;
        }
        else if (game->playersFull){
            message_send(from, "QUIT game is full.");
            mem_free(grid_msg);
            return false;
        }
        else {
            char c = playerNum;
            
            playerNum ++;
            if (playerNum > 90){
                game->playersFull = true;
            } 

            char* letter = mem_malloc(2);
            letter[0] = c;
            letter[1] = '\0';
            char name[51];
            for (int i=0; i< strlen(content); i++) {
                if (i == 50) {
                    break;
                }
                if (!isgraph(content[i]) && !isblank(content[i])) {
                    name[i] = '_';
                }
                else {
                    name[i] = content[i];
                }
            }
            char* nameMem = mem_calloc(strlen(content)+1, 1);
            strncpy(nameMem, name, strlen(content));
            // create new player
            player = grid_add_player(game->grid, letter, nameMem, from);
            mem_free(nameMem);
            set_insert(game->players, msg_str, player);
            set_insert(game->player_addresses, letter, player);
            mem_free(letter);
            char message[5] = "OK ";
            message[3] = c;
            message_send(from, message);

            // Send GRID message
            sprintf(grid_msg, "GRID %d %d", grid_get_rows(game->grid), grid_get_columns(game->grid));
            message_send(from, grid_msg);
            mem_free(grid_msg);
            set_iterate(game->players, NULL, sendAll);
            sendSpectateMessage();
            return false;
        }


    }

    // handle SPECTATE message
    else if (strstr(message, "SPECTATE") != NULL) {
        if (message_eqAddr(message_noAddr(), game->spectator)){
            game->spectator = from;
        }
        else {
            message_send(game->spectator, "QUIT Another spectator has joined. You have been booted from the server.");
            game->spectator = from;
        }

        // Send GRID message
        sprintf(grid_msg, "GRID %d %d", grid_get_rows(game->grid), grid_get_columns(game->grid));
        message_send(from, grid_msg);
        sendSpectateMessage();
    }

    else if (strstr(message, "KEY") != NULL) {
        char keyPressed = content[0];
        player = set_find(game->players, msg_str);
        switch(keyPressed) {
        case 'Q' :
            if (message_eqAddr(from, game->spectator)) {   // Check if client is a spectator
                message_send(from, "QUIT Thanks for watching!");
            }
            else {
                message_send(from, "QUIT Thanks for playing!");
                // get rid of player
                grid_set_objectOnTop(game->grid, player_get_row(player), player_get_column(player), '\0');
                // EXTRA CREDIT GOLD PILE DROP START
                if(player_get_gold(player) > 0){
                    grid_set_gold(game->grid, player_get_row(player), player_get_column(player), player_get_gold(player));
                    // set gold in spot where player quit to their gold
                    grid_set_gold_total(game->grid, grid_get_total_gold(game->grid) + player_get_gold(player));
                    // updating the total gold remaining on the ground
                    player_add_gold(player, -player_get_gold(player)); // set player's gold to 0
                }
                // EXTRA CREDIT GOLD PILE DROP END
            }
            break;
        case 'h' : movePlayer(player, game->grid, 0, -1); break; // left
        case 'l' : movePlayer(player, game->grid, 0, 1); break; // right
        case 'j' : movePlayer(player, game->grid, 1, 0); break; // down
        case 'k' : movePlayer(player, game->grid, -1, 0); break; // up
        case 'y' : movePlayer(player, game->grid, -1, -1); break; // up-left
        case 'u' : movePlayer(player, game->grid, -1, 1); break; // up-right
        case 'b' : movePlayer(player, game->grid, 1, -1); break; // down-left
        case 'n' : movePlayer(player, game->grid, 1, +1); break; // down-right
        case 'H' : while (movePlayer(player, game->grid, 0, -1)) { continue; }; break;
        case 'L' : while (movePlayer(player, game->grid, 0, 1)) { continue; }; break;
        case 'J' : while (movePlayer(player, game->grid, 1, 0)) { continue; }; break;
        case 'K' : while (movePlayer(player, game->grid, -1, 0)) { continue; }; break;
        case 'Y' : while (movePlayer(player, game->grid, -1, -1)) { continue; }; break;
        case 'U' : while (movePlayer(player, game->grid, -1, 1)) { continue; }; break;
        case 'B' : while (movePlayer(player, game->grid, 1, -1)) { continue; }; break;
        case 'N' : while (movePlayer(player, game->grid, 1, +1)) { continue; }; break;
        default: message_send(from, "ERROR usage: unknown keystroke"); mem_free(grid_msg); return false;
        }

        if (grid_get_total_gold(game->grid) == 0) {
            mem_free(grid_msg);
            gameOver();
            return true;
        }
        
        set_iterate(game->players, NULL, sendAll);
        sendSpectateMessage();

    }
    mem_free(grid_msg);
    return false;
}

static bool movePlayer(player_t* player, grid_t* grid, int rowShift, int colShift) {
    int goldFound;

    int row = player_get_row(player) + rowShift; // get the row we are trying to move to
    int column = player_get_column(player) + colShift; // get the column we are trying to move to
    if(!grid_isMovable(grid, row, column)){ // spot can not be moved to
        return false; // failed to move
    }
    if(grid_get_objectOnTop(grid, row, column) == '\0'){
        grid_set_objectOnTop(grid, player_get_row(player), player_get_column(player), '\0');
        // set objectOnTop to null in the spot the player just left
        player_set_location(player, row, column); // update player location
        grid_set_objectOnTop(grid, row, column, player_get_letter(player)); // set objectOnTop to player's letter
        grid_update_visibility(grid, player);
        return true;
    } else if(grid_get_gold(grid, row, column) > 0){ // has gold
        grid_set_objectOnTop(grid, player_get_row(player), player_get_column(player), '\0');
        // set objectOnTop to null in the spot the player just left
        player_set_location(player, row, column); // update player location
        grid_set_objectOnTop(grid, row, column, player_get_letter(player)); // set objectOnTop to player's letter in new spot
        if((goldFound = grid_take_gold(grid, row, column)) > 0) { // clear gold from spot
            player_set_gold_found(player, goldFound);
            player_add_gold(player, goldFound); // add gold to player
        }
        grid_update_visibility(grid, player);
        return true;
    } else{ // only other option is a letter for object on top
        char otherLetter[1];
        otherLetter[0] = grid_get_objectOnTop(grid, row, column);
        player_t* otherPlayer = set_find(game->player_addresses, otherLetter);
        // get player pointer from the letter (NEED TO MAKE THIS LETTERTOCHARACTER SET!!!)
        player_set_location(otherPlayer, player_get_row(player), player_get_column(player)); // update the other player's location
        grid_set_objectOnTop(grid, player_get_row(player), player_get_column(player), player_get_letter(otherPlayer));
        // set objectOnTop to the other player's letter in the spot the player just left
        grid_set_objectOnTop(grid, row, column, player_get_letter(player)); // set objectOnTop to player's letter in new spot
        player_set_location(player, row, column); // update player location
        grid_update_visibility(grid, player);
        // EXTRA CREDIT 3 START
        player_steal_gold(player, otherPlayer); // steal the gold from other player
        // EXTRA CREDIT 3 END

        return true;
    }
}

static void sendAll(void* arg, const char* key, void* item) {
    if (item != NULL){
        sendMessage(item);
    }
}

static void sendMessage(player_t* player) {
    char* gold_msg = mem_malloc(50); // allocate enough memory for the formatted message
    char disp_msg[65507] = "DISPLAY\n"; // Allocate maximum message size allowed
    addr_t address = player_get_address(player);

    // Send GOLD message
    sprintf(gold_msg, "GOLD %d %d %d", player_get_gold_found(player), player_get_gold(player), grid_get_total_gold(game->grid));
    // printf("\n here is gold mess: %s", gold_msg);
    message_send(address, gold_msg);
    
    // Send DISPLAY message
    grid_update_visibility(game->grid, player);
    char letter[2];
    letter[0] = player_get_letter(player);
    letter[1] = '\0';
    char* mapString = grid_get_map(game->grid, letter);
    strcat(disp_msg, mapString);
    // printf("\n here is display mess: %s", disp_msg);
    // strcat(disp_msg, grid_get_mapß(game->grid, letter));
    message_send(address, disp_msg);
    mem_free(gold_msg);
    mem_free(mapString);
}

static void sendSpectateMessage() {
    char* gold_msg = mem_malloc(50); // allocate enough memory for the formatted message
    char disp_msg[65507] = "DISPLAY\n"; // Allocate maximum message size allowed

    // Send GOLD message
    sprintf(gold_msg, "GOLD 0 0 %d", grid_get_total_gold(game->grid));
    message_send(game->spectator, gold_msg);
    
    // Send DISPLAY message
    char* mapString = grid_get_map(game->grid, NULL);
    strcat(disp_msg, mapString);
    message_send(game->spectator, disp_msg);
    //printf("\n here is display mess: %s", disp_msg);
    // strcat(disp_msg, grid_get_map(game->grid, NULL));
    // message_send(game->spectator, disp_msg);
    mem_free(gold_msg);
    mem_free(mapString);
}


static void gameOver() {
    char* player_msg = mem_calloc(1000, 1);

    // Iterate to create string of player results
    set_iterate(game->players, player_msg, result_print);
    char* end_msg = mem_malloc(strlen(player_msg) + 17);
    strcpy(end_msg, "QUIT GAME OVER:\n");
    strcat(end_msg, player_msg);

    // Send to all players in game
    set_iterate(game->players, end_msg, game_over_send);
    message_send(game->spectator, end_msg);
    mem_free(player_msg);
    mem_free(end_msg);
    set_delete(game->player_addresses, NULL);
    set_delete(game->players, NULL);
    grid_delete(game->grid);
    mem_free(game);
}

static void result_print(void* arg, const char* key, void* item) {
    char* msg = (char*)arg;
    char* player_msg = mem_malloc(1000);
    
    
    if (item != NULL) {
        player_t* player = (player_t*)item;
        sprintf(player_msg, "%c %9d %s\n", player_get_letter(player), player_get_gold(player), player_get_name(player));
        strncat(msg, player_msg, strlen(player_msg));
    }
    mem_free(player_msg);
}

static void game_over_send(void* arg, const char* key, void* item) {
    char* msg = arg;
    if (item != NULL) {
        player_t* player = item;
        addr_t address = player_get_address(player);
        message_send(address, msg);
        player_delete(player);
    }
}



/************************** send_these_messages *****************************/
// A “QUIT” message if the game is already full, or if the player’s name was not provided
// An “OK” message if the player has been added to the game
// A “GRID” message that sends the number of rows and number of columns to the client
// A “GOLD” message that contains information about the number of gold nuggets collected, the number of gold nuggets in a players purse, and how many nuggets are to be found
// A “DISPLAY” string which is a multi-textual representation of the grid as it is known/seen by the client




