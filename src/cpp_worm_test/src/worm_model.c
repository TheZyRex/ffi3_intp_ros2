// A simple variant of the game Snake
//
// Used for teaching in classes
//
// Author:
// Franz Regensburger
// Ingolstadt University of Applied Sciences
// (C) 2011
//
// The worm model

#include <curses.h>

#include "worm.h"
#include "board_model.h"
#include "worm_model.h"

// *****************************************************
// Functions concerning the management of the worm data
// *****************************************************

// START WORM_DETAIL
// The following functions all depend on the model of the worm

// Getters
struct pos getWormHeadPos(struct worm *aworm)
{
  // Structures are passed by value
  // -> returning a copy instead of reference
  return aworm->wormpos[aworm->headindex];
}

// Initialize the worm
resCode_t initializeWorm(struct worm *aworm, int len_max, int len_cur, struct pos headpos, wHeading_t dir, colorPair_t color) {
      
    // Initialize last usable index to len_max -1
    aworm->maxindex = len_max-1;

    // Current last usable index in array. May grow upto maxindex
    aworm->cur_lastindex = len_cur-1;
    
    // Initialize headindex
    aworm->headindex = 0;

    // Mark all elements as unused in the arrays of positions
    // An unused position in the array is marked
    // with code UNUSED_POS_ELEM
    for(size_t i = 0; i <= aworm->maxindex; i++)
    {
      aworm->wormpos[i].y = UNUSED_POS_ELEM;
      aworm->wormpos[i].x = UNUSED_POS_ELEM;
    }

    // Initialize position of worms head
    aworm->wormpos[aworm->headindex] = headpos;

    // Initialize the heading of the worm
    setWormHeading(aworm, dir);

    // Initialze color of the worm
    aworm->wcolor = color;

    return RES_OK;
}

// Function to cleanup tailindex in array if it's in use
void cleanWormTail(struct board *aboard, struct worm *aworm)
{
  //Compute tailindex
  int tailindex = (aworm->headindex + 1) % (aworm->cur_lastindex+1);

  // Check the array of worm elements
  // Is the array element at tailindex already in use?
  if(aworm->wormpos[tailindex].x != UNUSED_POS_ELEM || aworm->wormpos[tailindex].y != UNUSED_POS_ELEM)
  {
    //YES: place a SYMBOL_FREE_CELL at the tail's position
    placeItem(aboard, aworm->wormpos[tailindex].y, aworm->wormpos[tailindex].x, BC_FREE_CELL, SYMBOL_FREE_CELL, COLP_FREE_CELL);
  }

}

void showWorm(struct board *aboard, struct worm *aworm)
{
  int tailindex = (aworm->headindex + 1) % (aworm->cur_lastindex + 1);
  int i = aworm->headindex;

  do {
      if (i == aworm->headindex) 
      {
        placeItem(aboard, aworm->wormpos[i].y, aworm->wormpos[i].x, BC_USED_BY_WORM, SYMBOL_WORM_HEAD_ELEMENT, aworm->wcolor);
      }
      else if (i == tailindex)
      {
        // worm growing = tailindex cannot be set, as the tail index as no coordinates yet
        placeItem(aboard, aworm->wormpos[i].y, aworm->wormpos[i].x, BC_USED_BY_WORM, SYMBOL_WORM_TAIL_ELEMENT, aworm->wcolor);
      }
      else
      {
        placeItem(aboard, aworm->wormpos[i].y, aworm->wormpos[i].x, BC_USED_BY_WORM, SYMBOL_WORM_INNER_ELEMENT, aworm->wcolor);
      }

    i--; 

    if (i<0)
    {
      // Wurde der Wurm zuvor verlängert, dann wird im nächsten Durchlauf die Schleife beendet
      i = aworm->cur_lastindex;
    }
      
  } while (i != aworm->headindex && aworm->wormpos[i].x != UNUSED_POS_ELEM);

}

void moveWorm(struct board *aboard, struct worm *aworm, gameState_t * agame_state) {
    
    struct pos headpos; 

    // Get the current position of the worm's head element and
    headpos = aworm->wormpos[aworm->headindex];
    // compute the new head position according to current heading.
    // Do not store the new head position in the array of positions yet 
    headpos.x += aworm->dx;
    headpos.y += aworm->dy;

    // Check if we would leave the display if we move the worm's head according
    // to worm's last direction.
    // We are not allowed to leave the display's window.
    if (headpos.x < 0) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos.x > getLastColOnBoard(aboard) ) { 
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos.y < 0) {  
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else if (headpos.y > getLastRowOnBoard(aboard) ) {
        *agame_state = WORM_OUT_OF_BOUNDS;
    } else {
        // We will stay within bounds.
        switch ( getContentAt(aboard, headpos) ) {
          case BC_FOOD_1:
            *agame_state = WORM_GAME_ONGOING;
            growWorm(aworm, BONUS_1);
            decrementNumberOfFoodItems(aboard);
            break;
            
          case BC_FOOD_2:
            *agame_state = WORM_GAME_ONGOING;
            growWorm(aworm, BONUS_2);
            decrementNumberOfFoodItems(aboard);
            break;

          case BC_FOOD_3:
            *agame_state = WORM_GAME_ONGOING;
            growWorm(aworm, BONUS_3);
            decrementNumberOfFoodItems(aboard);
            break;
          
          case BC_BARRIER:
            *agame_state = WORM_CRASH;
            break;
            
          case BC_USED_BY_WORM:
            *agame_state = WORM_CROSSING;
            break;
            
          default:
            {;}
        }
    }
    
    // Check the status of *agame_state
    // Go on if nothing bad happened
    if(*agame_state == WORM_GAME_ONGOING)
    {
      // Increment theworm_headindex
      // Go round if end of worm is reached (ring buffer)
      aworm->headindex = (aworm->headindex + 1) % (aworm->cur_lastindex + 1);
      // Store new coordinates of head element in worm structure
      aworm->wormpos[aworm->headindex] = headpos;
    }
}

void growWorm(struct worm *aworm, enum Boni growth)
{
  // Play it safe and inhibit surpassing the bound
  if (aworm->cur_lastindex + growth <= aworm->maxindex)
  {
    //aworm->headindex = aworm->cur_lastindex;
    aworm->cur_lastindex += growth;
  }
  else
  {
    aworm->cur_lastindex = aworm->maxindex;
  }
}

// Getters

int getWormLength(struct worm *aworm)
{
  // +1 to get real length, not last array index
  return aworm->cur_lastindex+1;
}

// Setters
void setWormHeading(struct worm *aworm, wHeading_t dir) {
    switch(dir) {
        case WORM_UP :// User wants up
            aworm->dx=0;
            aworm->dy=-1;
            break;
        case WORM_DOWN :// User wants down
            aworm->dx=0;
            aworm->dy=+1;
            break;
        case WORM_LEFT      :// User wants left
            aworm->dx=-1;
            aworm->dy=0;
            break;
        case WORM_RIGHT      :// User wants right
            aworm->dx=+1;
            aworm->dy=0;
            break;
    }
} 