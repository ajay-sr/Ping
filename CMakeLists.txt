cmake_minimum_required(VERSION 3.16)
project(ping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build optimized unless a build type is chosen explicitly.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

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

if(WIN32)
    # SDL2main provides WinMain, so the game opens without a console window.
    target_link_libraries(ping PRIVATE SDL2::SDL2main)
    set_target_properties(ping PROPERTIES WIN32_EXECUTABLE ON)
endif()

if(MSVC)
    target_compile_options(ping PRIVATE /W4)
else()
    target_compile_options(ping PRIVATE -Wall -Wextra -Wpedantic)
endif()
