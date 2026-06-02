/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Criterion tests for the TCP Listener
*/

#include <criterion/criterion.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <optional>
#include <type_traits>

#include "server/network/Listener.hpp"
#include "server/network/NetworkError.hpp"
#include "server/posix/FileDescriptor.hpp"

namespace {

/// @brief Connect a blocking client socket to a loopback port.
int connect_loopback(std::uint16_t port)
{
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    cr_assert_neq(fd, -1, "client socket() failed");
    cr_assert_eq(::connect(fd, reinterpret_cast<sockaddr *>(&addr),
        sizeof(addr)), 0, "connect() failed");
    return fd;
}

/// @brief Poll accept_connection a few times to let the kernel settle.
std::optional<zappy::server::posix::FileDescriptor> accept_retry(
    const zappy::server::network::Listener &listener)
{
    for (int i = 0; i < 200; ++i) {
        std::optional<zappy::server::posix::FileDescriptor> client =
            listener.accept_connection();

        if (client)
            return client;
        ::usleep(1000);
    }
    return std::nullopt;
}

} // namespace

Test(listener, binds_os_assigned_port)
{
    zappy::server::network::Listener listener(0);

    cr_assert_neq(listener.port(), 0, "port 0 must resolve to a real port");
    cr_assert_geq(listener.fd(), 0);
}

Test(listener, accept_is_nullopt_when_idle)
{
    zappy::server::network::Listener listener(0);

    cr_assert_not(listener.accept_connection().has_value(),
        "no pending connection must yield nullopt");
}

Test(listener, accepts_a_client)
{
    zappy::server::network::Listener listener(0);
    int client = connect_loopback(listener.port());
    std::optional<zappy::server::posix::FileDescriptor> accepted =
        accept_retry(listener);

    cr_assert(accepted.has_value(), "listener should accept the client");
    cr_assert(accepted->is_valid());
    ::close(client);
}

Test(listener, peer_address_is_loopback)
{
    zappy::server::network::Listener listener(0);
    int client = connect_loopback(listener.port());
    std::optional<zappy::server::posix::FileDescriptor> accepted =
        accept_retry(listener);

    cr_assert(accepted.has_value());

    const zappy::server::posix::Address peer =
        zappy::server::network::Listener::peer_address(*accepted);

    cr_assert_str_eq(peer.ip().c_str(), "127.0.0.1");
    ::close(client);
}

Test(listener, is_move_only)
{
    cr_assert_not(std::is_copy_constructible_v<
        zappy::server::network::Listener>);
    cr_assert(std::is_move_constructible_v<
        zappy::server::network::Listener>);
}
