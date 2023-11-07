// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The board model
#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "worm.h"
#include "board_model.h"
#include "messages.h"


// *************************************************
// Placing and removing items from the game board
// Check boundaries of game board
// *************************************************

// Place an item onto the curses display.
// chtype was declared by typedef in the ncurses.h library. 
void placeItem(struct board *aboard, int y, int x, enum BoardCodes board_code,
    chtype symbol,  colorPair_t color_pair) {

    aboard->cells[y][x] = board_code;
    //  Store item on the display (symbol code)
    move(y, x);                         // Move cursor to (y,x)
    attron(COLOR_PAIR(color_pair));     // Start writing in selected color
    addch(symbol);                      // Store symbol on the virtual display
    attroff(COLOR_PAIR(color_pair));    // Stop writing in selected color
}

resCode_t initializeBoard(struct board *aboard)
{
  // Maximal index of a row / col
  aboard->last_row =  LINES - ROWS_RESERVED - 1; 
  aboard->last_col =  COLS - 1;

  // Check dimenstions of the board
  if ( aboard->last_col < MIN_NUMBER_OF_COLS - 1 || aboard->last_row < MIN_NUMBER_OF_ROWS - 1)
  {
    char buf[100];
    sprintf(buf, "Das Fenster ist zu klein: wir brauchen %dx%d", MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS + ROWS_RESERVED);
    showDialog(buf, "Bitte eine Taste druecken");
    return RES_FAILED;
  }

  // For LINES amount of rows, a size of "sizeof(enum BoardCodes*)" bytes is
  // initialized and allocated to aboard->cells which is a APA structure
  // the calloc() function allocates memory for an array of nmemb elements of size bytes each and returns a pointer
  // to the allocated memory
  aboard->cells = calloc(LINES-1, sizeof(enum BoardCodes*));

  if (aboard->cells == NULL)
  {
    showDialog("Abbruch: Zu wenig Speicher", "Bitte eine Taste druecken");
    exit(RES_FAILED); // No memory -> direct exit
  }

  for (int y = 0; y < LINES-1; y++)
  {
    // Allocate array of columns for each y
    // If you need the dynamically allocated memory to be zero-initialized, then use calloc, else use malloc
    // in this case i use malloc, because i dont need to initialize every col
    aboard->cells[y] = malloc((COLS-1) * sizeof(int*));
    if ( aboard->cells[y] == NULL)
    {
      showDialog("Abbruch: Zu wenig Speicher", "Bitte eine Taste druecken");
      exit(RES_FAILED); // No memory -> direct exit
    }
  }


  return RES_OK;
}

resCode_t initializeLevel(struct board *aboard)
{
  int y,x;
  // Init seed for rand function
  srand(time(0));
  // Fill board and screen buffer with empty cells.
  for (y = 0; y <= aboard->last_row; y++) {
    for (x = 0; x <= aboard->last_col; x++) {
      placeItem(aboard, y, x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
    }
  }

  // Draw a line in order to separate the message area
  // Note: we cannot use function placeItem() since the message area
  // is outside the board!
  y = aboard->last_row + 1;

  for (x = 0; x <= aboard->last_col; x++) {
    move(y, x);
    attron(COLOR_PAIR(COLP_BARRIER));
    addch(SYMBOL_BARRIER);
    attroff(COLOR_PAIR(COLP_BARRIER));
  }

  //// Draw a line to signal the rightmost column of the board.
  //for (y = 0; y <= aboard->last_row ; y++) {
  //  placeItem(aboard,y,aboard->last_col,
  //  BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
  //}

  // Barriers: use a loop
  x = aboard->last_col/3; 
  y = 10;
  for (int i = 0; i < (rand()%(15-10+1))+10; i++, y++) {
    placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
  }

  x = 2*(aboard->last_col/3);
  y = 5;
  for (int i = 0; i < (rand()%(15-10+1))+10; i++, y++) {
    placeItem(aboard,y,x,BC_BARRIER,SYMBOL_BARRIER,COLP_BARRIER);
  }

  // Food
  // Set random amount off food items
  aboard->food_items = (rand() % (10-5+1)) + 5;

  for(int i = 0; i < aboard->food_items; i++)
  {
    // Initialize random x and y coordinates in a specific range
    int randx = rand() % aboard->last_col; 
    int randy = rand() % aboard->last_row; 

    // Continue to create new coords until board cell is a free cell
    while(aboard->cells[randy][randx] != BC_FREE_CELL)
    {
      randx = rand() % aboard->last_col; 
      randy = rand() % aboard->last_row; 
    }
    
    // chose random food item
    switch(rand()%3) {
      case 0:
        placeItem(aboard, randy, randx, BC_FOOD_1, SYMBOL_FOOD_1, COLP_FOOD_1);
        break;
      case 1: 
        placeItem(aboard, randy, randx, BC_FOOD_2, SYMBOL_FOOD_2, COLP_FOOD_2);
        break;
      case 2: 
        placeItem(aboard, randy, randx, BC_FOOD_3, SYMBOL_FOOD_3, COLP_FOOD_3);
        break;
    }
      
  }
    
  return RES_OK;
}

// Cleanup Board

void cleanupBoard(struct board *aboard)
{
  for (int y = 0; y < LINES-1; y++)
  {
    // if cells[y] is NULL we can presume that the following elements are also NULL
    if (aboard->cells[y] == NULL)
    {
      break;
    }
    free(aboard->cells[y]);
  }
  free(aboard->cells);
}

// Getters

int getLastRowOnBoard(struct board *aboard)
{
  return aboard->last_row;
}

int getLastColOnBoard(struct board *aboard)
{
  return aboard->last_col;
}

int getNumberOfFoodItems(struct board *aboard)
{
  return aboard->food_items;
}

enum BoardCodes getContentAt(struct board *aboard, struct pos position)
{
  // cells[ROW][COLS] as position is not a pointer. x and y are accessed by . and not by ->
  return aboard->cells[position.y][position.x]; 
}

// Setters
void decrementNumberOfFoodItems(struct board *aboard)
{
  // Would work, but we learned to not do that...
  // aboard->food_items--;
  
  aboard->food_items = aboard->food_items - 1;
}

void setNumberOfFoodItems(struct board *aboard, int n)
{
  aboard->food_items = n;
}