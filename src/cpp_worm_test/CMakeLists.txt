cmake_minimum_required(VERSION 3.8)
project(cpp_worm_test)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(
    include
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Curses REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(worm_game
    src/worm_node.cpp
    src/prep.c
    src/worm_model.c
    src/board_model.c
    src/messages.c
    src/worm.c
)
ament_target_dependencies(worm_game Curses rclcpp)

install(TARGETS 
    worm_game
    DESTINATION lib/${PROJECT_NAME}
)

target_link_libraries(worm_game
    ncurses  # Link to the ncurses library
)

ament_package()
