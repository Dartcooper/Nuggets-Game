/* Jorie MacDonald, Cooper LaPorte, Riley Johnson, Arun Guruswamy, Dartmouth 23W, CS50
 * client.c - Client program for CS50 gold nuggets game
 *
 * usage: Provides a user interface to play the gold nuggets game
 *
 * Exit Codes: 1 - Can't connect to server, 2 - Invalid number of arguments, 3 - Invalid port number,
 *             4 - Could not initialize message module
 *
 * CS50, Winter 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <message.h>
#include <log.h>
#include <ncurses.h>
#include "mem.h"

static void parseArgs(int argc, char* argv[]); 
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message); 
static void checkWindowSize(const char* message);

typedef struct clientInfo{
  char player_letter;
  int goldMessageLength;
} clientInfo_t;

// Initialize new clientInfo
static clientInfo_t* clientInfo;

int main(const int argc, char* argv[]) {
  char play_message[150] = "PLAY ";

  parseArgs(argc, argv);

  initscr(); // initialize the screen
  cbreak();  // accept keystrokes immediately
  noecho();
  start_color(); // turn on color controls
  init_pair(1, COLOR_RED, COLOR_BLACK); // define color 1
  attron(COLOR_PAIR(1)); // set the screen color using color 1
  refresh();

  // commandline provides address for server
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  addr_t server; // address of the server
  if (!message_setAddr(serverHost, serverPort, &server)) {
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    exit(1); // bad hostname/port
  }

  // Evaluate mode once arguments are validated
  if (argc == 3) {   // Spectate mode
    message_send(server, "SPECTATE");
  }
  else if (argc == 4) {  // Player mode
    strcat(play_message, argv[3]);
    message_send(server, play_message);
  }

  // Create clientInfo
  clientInfo = mem_malloc(sizeof(clientInfo_t));
  clientInfo->player_letter = '*';
  clientInfo->goldMessageLength = 0;

  // Read messages from server until game is over
  message_loop(&server, 0, NULL, handleInput, handleMessage);

  // shut down the message module
  message_done();

  mem_free(clientInfo);
}

// parses and validates arguments provided through the command-line, intializes message module
static void parseArgs(const int argc, char* argv[]) {

  // Check number of arguments
  if (argc != 3 && argc != 4) {
    printf("%d", argc);
    fprintf(stderr, "usage: client hostname port\n");
    exit(2);
  }

  // Check port
  int port = atoi(argv[2]);

  if (port < 1 || port > 65536) {
    fprintf(stderr, "Please enter a valid port number");
    exit(3);
  }

  // initialize the message module (without logging)
  if (message_init(NULL) == 0) {
    exit(4); // failure to initialize message module
  }

}

// look for keyboard input to send to server
static bool handleInput(void* arg) {
  char c;
  char message[65512] = "KEY ";

  addr_t* serverp = arg;
  if (serverp == NULL) {
    fprintf(stderr, "handleInput called with arg=NULL");
    return true;
  }
  if (!message_isAddr(*serverp)) {
    fprintf(stderr, "handleInput called without a correspondent.");
    return true;
  }
  
  // allocate a buffer into which we can read a line of input
  // (it can't be any longer than a message)!

  c = getch();
  // read a line from stdin
  //if (fgets(line, message_MaxBytes, stdin) == NULL) {
  if (c == EOF) { 
    message_send(*serverp, "KEY Q");
    return false; 
  }
  else {
    message[4] = c;

    // send as message to server
    message_send(*serverp, message);

    // normal case: keep looping
    return false;
  }

}

// Handle messages from Server
static bool handleMessage(void* arg, const addr_t from, const char* message) { 
  int gold_collected;
  int purse; 
  int gold_remaining;
  char* substring;  // Alloc mem?

  // Check type of message from Server
  if (strstr(message, "OK") != NULL) {
    if ((substring = strchr(message, ' ')) != NULL) {
      clientInfo->player_letter = substring[1];
    }
    else {
      fprintf(stderr, "Invalid OK message");
    }
  }

  else if (strstr(message, "GRID") != NULL) {
    checkWindowSize(message);
  }

  else if (strstr(message, "DISPLAY") != NULL) {
    substring = strchr(message, '\n');
    if(substring != NULL) {
      mvprintw(1, 0, "%s\n", substring);
    }
    else {
      fprintf(stderr, "Invalid DISPLAY message");
    }
  }

  else if (strstr(message, "GOLD") != NULL) {
    substring = strchr(message, ' ');
    int maxMessageSize = 80;
    char goldMessage[maxMessageSize]; 
    if (sscanf(substring, "%d %d %d", &gold_collected, &purse, &gold_remaining) == 3) {
      move(0, 0);
      clrtoeol();
      if (clientInfo->player_letter == '*') {
        snprintf(goldMessage, maxMessageSize, "Spectator: %d nuggets unclaimed",
            gold_remaining); // Spectator case
        clientInfo->goldMessageLength = strlen(goldMessage);
        printw("%s", goldMessage);
      }
      else if (gold_collected == 0) {
        snprintf(goldMessage, maxMessageSize, 
            "Player %c has %d nuggets (%d nuggets unclaimed).", 
            clientInfo->player_letter, purse, gold_remaining);
        clientInfo->goldMessageLength = strlen(goldMessage);
        printw("%s", goldMessage);
      }
      else {
        snprintf(goldMessage, maxMessageSize, 
            "Player %c has %d nuggets (%d nuggets unclaimed).", 
            clientInfo->player_letter, purse, gold_remaining);
        clientInfo->goldMessageLength = strlen(goldMessage);
        printw("%s GOLD received: %d", goldMessage, gold_collected);
      }
    }
    else {
      fprintf(stderr, "Invalid GOLD message");
    }
  }

  else if (strstr(message, "QUIT") != NULL) { 
    substring = strchr(message, ' ');
    if(substring != NULL) {
      // Close window
      endwin();
      printf("%s\n", substring);
    }
    else {
      fprintf(stderr, "Invalid QUIT message");
    } 
    return true;
  }

  else if (strstr(message, "ERROR") != NULL) {
    move(0, clientInfo->goldMessageLength);
    clrtoeol();
    substring = strchr(message, ' ');
    if(substring != NULL) {
      printw("%s\n", substring);
    }
    else {
      fprintf(stderr, "Invalid ERROR message");
    } 
  }

  refresh();
  return false;

}

// Handle first message from server to check if client window is larger than server map size
static void checkWindowSize(const char* message) {
  int map_width = 0;
  int map_height = 0;
  int win_width;
  int win_height = 0;
  char test[150];
  char c;

  // Parse numbers from message
  if (sscanf(message, "%s %d %d", test, &map_height, &map_width) != 3) {
    fprintf(stderr, "Invalid GRID Message");
  }

  getmaxyx(stdscr, win_height, win_width);

  while (win_width <= map_width || win_height <= map_height) {
    move(0, 0);
    printw("Your window must be at least %d high\n", map_height);
    printw("Your window must be at least %d wide\n", map_width);
    printw("Resize your window, and press Enter to continue.\n");


    if ((c = getch()) == '\n') {
      getmaxyx(stdscr, win_height, win_width);
    }

    refresh();
}

}





