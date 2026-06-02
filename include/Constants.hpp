/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Project-wide server constants
*/

#ifndef SERVER_CONSTANTS_HPP_
    #define SERVER_CONSTANTS_HPP_

namespace zappy::server {

/// @brief Process exit status on a fatal error (subject requirement).
constexpr int FAILURE = 84;

/// @brief Process exit status on success.
constexpr int SUCCESS = 0;

constexpr int INVALID_FD = -1;

}

#endif /* !SERVER_CONSTANTS_HPP_ */
