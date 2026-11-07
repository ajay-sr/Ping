cmake_minimum_required(VERSION 3.16)
project(ping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

add_executable(ping
    src/main.cpp
    src/Ball.cpp
    src/Config.cpp
    src/Game.cpp
    src/Jukebox.cpp
    src/Player.cpp
    src/ScoreBoard.cpp
    src/Text.cpp
    src/TitleScreen.cpp
)
target_link_libraries(ping PRIVATE SDL2::SDL2)
target_compile_options(ping PRIVATE -Wall -Wextra -Wpedantic)
