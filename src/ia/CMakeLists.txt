##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## AI CMakeLists
##

add_custom_target(zappy_ai ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/main.py
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_ai
    COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zappy_ai
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main.py
    COMMENT "Building zappy_ai"
)
