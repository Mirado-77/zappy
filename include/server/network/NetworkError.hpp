/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Typed exception for network layer failures
*/

#ifndef NETWORK_NETWORKERROR_HPP_
    #define NETWORK_NETWORKERROR_HPP_

    #include <stdexcept>
    #include <string>

namespace zappy::server::network {

/**
 * @brief Exception signalling a network layer failure.
 */
class NetworkError : public std::runtime_error {
public:
    /**
     * @brief Build an error with a reason.
     *
     * @param reason description of the failure
     */
    explicit NetworkError(const std::string &reason)
        : std::runtime_error(reason)
    {
    }
};

} // namespace zappy::server::network

#endif /* !NETWORK_NETWORKERROR_HPP_ */
