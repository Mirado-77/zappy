/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Value wrapper over sockaddr_in usable with POSIX socket APIs
*/

#ifndef POSIX_ADDRESS_HPP_
    #define POSIX_ADDRESS_HPP_

    #include <netinet/in.h>
    #include <sys/socket.h>

    #include <cstdint>
    #include <string>

namespace zappy::server::posix {

/// @brief Copyable value wrapper over an IPv4 sockaddr_in.
class Address {
public:
    /// @brief Build from a dotted IPv4 string; throws std::invalid_argument.
    Address(const std::string &ip, std::uint16_t port);

    /// @brief Build from an existing sockaddr_in.
    explicit Address(const sockaddr_in &raw) noexcept;

    /// @brief Build a wildcard address (INADDR_ANY) on @p port.
    [[nodiscard]] static Address any(std::uint16_t port) noexcept;

    /// @return The dotted IPv4 string.
    [[nodiscard]] std::string ip() const;

    /// @return The port in host byte order.
    [[nodiscard]] std::uint16_t port() const noexcept;

    /// @return The underlying sockaddr_in.
    [[nodiscard]] const sockaddr_in &raw() const noexcept;

    /// @return The address as a generic sockaddr for bind/connect/accept.
    [[nodiscard]] const sockaddr *raw_generic() const noexcept;

    /// @return The size of the underlying sockaddr_in.
    [[nodiscard]] socklen_t size() const noexcept;

private:
    sockaddr_in _addr;
};

}

#endif /* !POSIX_ADDRESS_HPP_ */
