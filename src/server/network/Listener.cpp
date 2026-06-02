/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Implementation of the RAII TCP listener
*/

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include "server/network/Listener.hpp"
#include "server/network/NetworkError.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>


/// @brief Create a non-blocking listening socket bound to INADDR_ANY.
static zappy::server::posix::FileDescriptor makeListeningSocket(
    std::uint16_t port, int backlog)
{
    zappy::server::posix::FileDescriptor sock(
        ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
    const int yes = 1;

    if (!sock.is_valid())
        throw zappy::server::network::NetworkError(
            std::string("socket: ") + std::strerror(errno));
    if (::setsockopt(sock.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        throw zappy::server::network::NetworkError(
            std::string("setsockopt: ") + std::strerror(errno));

    const zappy::server::posix::Address addr =
        zappy::server::posix::Address::any(port);

    if (::bind(sock.get(), addr.raw_generic(), addr.size()) < 0)
        throw zappy::server::network::NetworkError(
            std::string("bind: ") + std::strerror(errno));
    if (::listen(sock.get(), backlog) < 0)
        throw zappy::server::network::NetworkError(
            std::string("listen: ") + std::strerror(errno));
    return sock;
}

/// @brief Read the locally bound port (resolves OS-assigned port 0).
static std::uint16_t resolvePort(
    const zappy::server::posix::FileDescriptor &sock)
{
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);

    if (::getsockname(sock.get(), reinterpret_cast<sockaddr *>(&addr), &len) < 0)
        throw zappy::server::network::NetworkError(
            std::string("getsockname: ") + std::strerror(errno));
    return ntohs(addr.sin_port);
}

/// @brief Create, bind and listen on @p port.
zappy::server::network::Listener::Listener(std::uint16_t port, int backlog)
    : _socket(makeListeningSocket(port, backlog)), _port(resolvePort(_socket))
{
}

/// @brief The raw listening descriptor.
int zappy::server::network::Listener::fd() const noexcept
{
    return _socket.get();
}

/// @brief The port actually bound.
std::uint16_t zappy::server::network::Listener::port() const noexcept
{
    return _port;
}

/// @brief Accept one pending connection without blocking.
std::optional<zappy::server::posix::FileDescriptor>
zappy::server::network::Listener::accept_connection() const
{
    const int client = ::accept4(_socket.get(), nullptr, nullptr, SOCK_NONBLOCK);

    if (client < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return std::nullopt;
        throw NetworkError(std::string("accept: ") + std::strerror(errno));
    }
    return zappy::server::posix::FileDescriptor(client);
}

/// @brief Resolve the peer address of a connected socket.
zappy::server::posix::Address zappy::server::network::Listener::peer_address(
    const zappy::server::posix::FileDescriptor &client)
{
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);

    if (::getpeername(client.get(), reinterpret_cast<sockaddr *>(&addr),
            &len) < 0)
        throw NetworkError(std::string("getpeername: ") + std::strerror(errno));
    return zappy::server::posix::Address(addr);
}
