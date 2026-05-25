##
## EPITECH PROJECT, 2025
## C++ Project Template
## File description:
## CMakeLists template
##

cmake_minimum_required(VERSION 3.20)

project(CACA VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")

include_directories(include)

# Main compilation
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Tests
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    find_library(CRITERION_LIB criterion REQUIRED)

    file(GLOB_RECURSE TEST_FILES tests/*.cpp)

    file(GLOB_RECURSE SRC_NO_MAIN src/*.cpp)
    list(FILTER SRC_NO_MAIN EXCLUDE REGEX ".*main\\.cpp$")

    add_executable(unit_tests ${TEST_FILES} ${SRC_NO_MAIN})
    target_link_libraries(unit_tests ${CRITERION_LIB})
    target_link_options(unit_tests PRIVATE --coverage)
    target_compile_options(unit_tests PRIVATE --coverage)

    add_test(NAME unit_tests COMMAND unit_tests)
endif()