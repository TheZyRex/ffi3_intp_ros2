#include "rclcpp/rclcpp.hpp"
#include <ncurses.h>

extern "C" 
{
#include "worm.h"
#include "worm.c"
#include "prep.h"
}

int main(int argc, char **argv)
{
    resCode_t res_code;
    rclcpp::init(argc, argv);

    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("worm_game");

    initializeCursesApplication();  // Init various settings of our application
    initializeColors();             // Init colors used in the game

    if ( LINES < ROWS_RESERVED + MIN_NUMBER_OF_ROWS || COLS < MIN_NUMBER_OF_COLS ) {
        cleanupCursesApp();

        printf("Das Fenster ist zu klein: wir brauchen mindestens %dx%d\n",
                MIN_NUMBER_OF_COLS, MIN_NUMBER_OF_ROWS + ROWS_RESERVED );

        res_code = RES_FAILED;

    } else {
        res_code = doLevel();
        cleanupCursesApp();
    }

    return res_code;

    rclcpp::spin(node);
    rclcpp::shutdown();

  return 0;
} 