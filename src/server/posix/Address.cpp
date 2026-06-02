/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Implementation of the sockaddr_in value wrapper
*/

#include "server/posix/Address.hpp"

#include <arpa/inet.h>

#include <array>
#include <cstdint>
#include <stdexcept>

/// @brief Zero-initialised IPv4 sockaddr_in bound to @p port (host order).
static sockaddr_in makeBase(std::uint16_t port) noexcept
{
    sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    return addr;
}

/// @brief Build from a dotted IPv4 string; validates the IP.
zappy::server::posix::Address::Address(const std::string &ip,
    std::uint16_t port) : _addr(makeBase(port))
{
    if (inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) != 1)
        throw std::invalid_argument("invalid IPv4 address: " + ip);
}

/// @brief Build from an existing sockaddr_in.
zappy::server::posix::Address::Address(const sockaddr_in &raw) noexcept
    : _addr(raw)
{
}

/// @brief Build a wildcard address (INADDR_ANY) on @p port.
zappy::server::posix::Address
zappy::server::posix::Address::any(std::uint16_t port) noexcept
{
    sockaddr_in addr = makeBase(port);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return Address(addr);
}

/// @brief Return the dotted IPv4 string.
std::string zappy::server::posix::Address::ip() const
{
    std::array<char, INET_ADDRSTRLEN> buffer{};

    inet_ntop(AF_INET, &_addr.sin_addr, buffer.data(), buffer.size());
    return std::string(buffer.data());
}

/// @brief Return the port in host byte order.
std::uint16_t zappy::server::posix::Address::port() const noexcept
{
    return ntohs(_addr.sin_port);
}

/// @brief Return the underlying sockaddr_in.
const sockaddr_in &zappy::server::posix::Address::raw() const noexcept
{
    return _addr;
}

/// @brief Return the address as a generic sockaddr.
const sockaddr *zappy::server::posix::Address::raw_generic() const noexcept
{
    return reinterpret_cast<const sockaddr *>(&_addr);
}

/// @brief Return the size of the underlying sockaddr_in.
socklen_t zappy::server::posix::Address::size() const noexcept
{
    return static_cast<socklen_t>(sizeof(_addr));
}
