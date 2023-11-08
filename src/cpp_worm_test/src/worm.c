// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "prep.h"
#include "worm.h"
#include "worm_model.h"
#include "board_model.h"
#include "messages.h"



// ********************************************************************************************
// Forward declarations of functions
// ********************************************************************************************
// Management of the game
void readUserInput(struct worm *aworm, gameState_t * agame_state );
resCode_t doLevel();

// ********************************************************************************************
// Functions
// ********************************************************************************************
// Initialize colors of the game
void initializeColors()
{
  start_color();
  init_pair(COLP_USER_WORM, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLP_FREE_CELL, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLP_FOOD_1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLP_FOOD_2, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLP_FOOD_3, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLP_BARRIER, COLOR_RED, COLOR_BLACK);
}

void readUserInput(struct worm *aworm, gameState_t * agame_state ) {
    int ch; // For storing the key codes

    if ((ch = getch()) > 0) {
        // Blocking or non-blocking depends of config of getch
        switch(ch) {
            case 'q' :    // User wants to end the show
                *agame_state = WORM_GAME_QUIT;
                break;

            //User wants to move up
            case 'w':
            case KEY_UP :
                setWormHeading(aworm, WORM_UP);
                break;

            //User wants to move down
            case 's':
            case KEY_DOWN :
                setWormHeading(aworm, WORM_DOWN);
                break;

            //User wants to move left
            case 'a':
            case KEY_LEFT :
                setWormHeading(aworm, WORM_LEFT);
                break;

            //User wants to move right
            case 'd':
            case KEY_RIGHT :
                setWormHeading(aworm, WORM_RIGHT);
                break;

            //Cheat mode for single stepping
            case 'c' : 
                nodelay(stdscr, false);
                break;

            //Terminate cheat mode, make getch none blocking again
            case ' ' : 
                nodelay(stdscr, true);
                break;

            case 'g' :
                growWorm(aworm, BONUS_3);
                break;
        }
    }
    return;
}

resCode_t doLevel() {
    struct worm userworm, *ptr_userworm=NULL;   // Local variable for storing the user's worm
    ptr_userworm = &userworm;                   // Pointer for userworm
    struct board theboard, *ptr_theboard=NULL;
    ptr_theboard = &theboard;

    gameState_t game_state;     // The current game_state

    resCode_t res_code;     // Result code from functions
    bool end_level_loop;    // Indicates whether we should leave the main loop

    struct pos bottomLeft;


    // At the beginnung of the level, we still have a chance to win
    game_state = WORM_GAME_ONGOING;

    // Setup the board
    res_code = initializeBoard(ptr_theboard);
    if ( res_code != RES_OK)
    {
      return res_code;
    }

    // Initialize the current level
    res_code = initializeLevel(ptr_theboard);
    if ( res_code != RES_OK)
    {
      return res_code;
    }

    // There is always an initialized user worm.
    // Initialize the userworm with its size, position, heading.
    bottomLeft.y =  getLastRowOnBoard(ptr_theboard);
    bottomLeft.x =  0;

    res_code = initializeWorm(ptr_userworm, WORM_LENGTH, WORM_INITIAL_LENGTH, bottomLeft, WORM_RIGHT, COLP_USER_WORM);
    if ( res_code != RES_OK) {
        return res_code;
    }
    
    // Show worm at its initial position
    showWorm(ptr_theboard, ptr_userworm);
    
    // Display all what we have set up until now
    refresh();

    // Start the loop for this level
    end_level_loop = false; // Flag for controlling the main loop
    while(!end_level_loop) {
        // Process optional user input
        readUserInput(ptr_userworm, &game_state); 
        if ( game_state == WORM_GAME_QUIT ) {
            end_level_loop = true;
            continue; // Go to beginning of the loop's block and check loop condition
        }

        // Process userworm
        // Clean the tail of the worm
        cleanWormTail(ptr_theboard, ptr_userworm);
        // Now move the worm for one step
        moveWorm(ptr_theboard, ptr_userworm, &game_state);
        // Bail out of the loop if something bad happened
        if ( game_state != WORM_GAME_ONGOING ) {
            end_level_loop = true; 
            continue; // Go to beginning of the loop's block and check loop condition
        }
        // Show the worm at its new position
        showWorm(ptr_theboard, ptr_userworm);
        // END process userworm
        // Inform user about position and length of userworm in status window
        showStatus(ptr_theboard, ptr_userworm);

        // Sleep a bit before we show the updated window
        napms(NAP_TIME);

        // Display all the updates
        refresh();

        // Are we done with that level=
        if (getNumberOfFoodItems(ptr_theboard) == 0)
        {
          end_level_loop = true;
        }

        // Start next iteration
    }

    // Preset res_code for rest of the function
    res_code = RES_OK;

    // For some reason we left the control loop of the current level.
    switch (game_state) {
        case WORM_GAME_ONGOING:
          if (getNumberOfFoodItems(ptr_theboard) == 0)
          {
            showDialog("Sie haben diese Runde erfolgreich beendet!!!",
                "Bitte Taste druecken");
          }
          else
          {
            showDialog("Interner Fehler!", "Bitte Taste druecken");
            res_code = RES_INTERNAL_ERROR;
          }
          break;

        case WORM_GAME_QUIT:
            showDialog("Sie haben die aktuelle Runde abgebrochen!",
                "Bitte Taste druecken");
            break;

        case WORM_CRASH:
            showDialog("Sie haben das Spiel verloren, weil Sie in die Barriere gefahren sind",
                "Bitte Taste druecken");
            break;

        case WORM_OUT_OF_BOUNDS:
            showDialog("Sie haben das Spiel verloren, weil Sie das Spielfeld verlassen haben",
                "Bitte Taste druecken");
            break;

        case WORM_CROSSING:
            showDialog("Sie haben das Spiel verloren, weil Sie einen Wurm gekreuzt haben",
                "Bitte Taste druecken");
            break;

        default:
            showDialog("Interner Fehler!", "Bitte Taste druecken");
            res_code = RES_INTERNAL_ERROR;
    }

    // Normal exit point
    return res_code;
}

// END WORM_DETAIL
// ********************************************************************************************

// ********************************************************************************************
// MAIN
// ********************************************************************************************

//int main(void) {
//    resCode_t res_code;         // Result code from functions
//
//    getchar();
//
//    // Here we start
//    initializeCursesApplication();  // Init various settings of our application
//    initializeColors();             // Init colors used in the game
//
//    // Maximal LINES and COLS are set by curses for the current window size.
//    // Note: we do not cope with resizing in this simple examples!
//
//    // Check if the window is large enough to display messages in the message area
//    // a has space for at least one line for the worm
//    if ( LINES < ROWS_RESERVED + MIN_NUMBER_OF_ROWS || COLS < MIN_NUMBER_OF_COLS ) {
//        cleanupCursesApp();
//
//        printf("Das Fenster ist zu klein: wir brauchen mindestens %dx%d\n",
//                MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS + ROWS_RESERVED );
//
//        res_code = RES_FAILED;
//
//    } else {
//        res_code = doLevel();
//        cleanupCursesApp();
//    }
//
//    return res_code;
//}
//