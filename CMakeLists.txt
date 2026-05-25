##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Root CMakeLists
##

cmake_minimum_required(VERSION 3.20)

project(zappy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(src/server)
add_subdirectory(src/gui)
add_subdirectory(src/ia)

add_custom_target(fclean
    COMMAND ${CMAKE_COMMAND} -E rm -f
        ${CMAKE_SOURCE_DIR}/zappy_server
        ${CMAKE_SOURCE_DIR}/zappy_gui
        ${CMAKE_SOURCE_DIR}/zappy_ai
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMENT "Removing zappy binaries"
)
