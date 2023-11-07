// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//

#ifndef _WORM_H
#define _WORM_H


// Dimensions and bounds
#define NAP_TIME    100   // Time in milliseconds to sleep between updates of display
#define ROWS_RESERVED 4   // Lines reserved for the status area + 1 for the separator line
#define MIN_NUMBER_OF_ROWS 26  // The guaranteed number of rows available for the board
#define MIN_NUMBER_OF_COLS 70  // The guaranteed number of columns available for the board

// Symbols to display
#define SYMBOL_FREE_CELL  ' '
#define SYMBOL_BARRIER  '#'
#define SYMBOL_FOOD_1   '2'
#define SYMBOL_FOOD_2   '4'
#define SYMBOL_FOOD_3   '6'
#define SYMBOL_WORM_HEAD_ELEMENT '0'
#define SYMBOL_WORM_INNER_ELEMENT 'o'
#define SYMBOL_WORM_TAIL_ELEMENT '`'

// Numbers for color pairs used by curses macro COLOR_PAIR
enum ColorPairs {
    COLP_USER_WORM = 1,
    COLP_FREE_CELL,
    COLP_FOOD_1,
    COLP_FOOD_2,
    COLP_FOOD_3,
    COLP_BARRIER
};

// Result codes of functions
enum ResCodes {
    RES_OK,
    RES_FAILED,
    RES_WRONG_OPTION,
    RES_INTERNAL_ERROR,
};

// Game state codes
enum GameStates {
    WORM_GAME_ONGOING,
    WORM_CRASH,           // Crashed into a barrier
    WORM_OUT_OF_BOUNDS,   // Left screen
    WORM_CROSSING,        // Worm head crossed another worm element
    WORM_GAME_QUIT,       // User likes to quit
};

// Typedefs for enums
typedef enum GameStates gameState_t;
typedef enum ColorPairs colorPair_t;
typedef enum ResCodes resCode_t;

// Function declarations

//extern void initializeColors();
//extern void readUserInput(struct worm *aworm, gameState_t *agame_state );
//extern resCode_t doLevel(struct game_options *somegops);
//extern resCode_t playGame(int argc, char *argv[]);

#endif  // #define _WORM_H