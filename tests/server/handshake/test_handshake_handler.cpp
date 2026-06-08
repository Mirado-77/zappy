/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Unit tests for HandshakeHandler
*/

#include <criterion/criterion.h>
#include <fcntl.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "server/cli/ServerConfig.hpp"
#include "server/client/Client.hpp"
#include "server/client/ClientState.hpp"
#include "server/game/TeamRegistry.hpp"
#include "server/handshake/HandshakeHandler.hpp"
#include "server/posix/FileDescriptor.hpp"

using zappy::server::cli::ServerConfig;
using zappy::server::client::Client;
using zappy::server::client::ClientState;
using zappy::server::game::TeamRegistry;
using zappy::server::handshake::HandshakeHandler;
using zappy::server::handshake::HandshakeResult;
using zappy::server::posix::FileDescriptor;

/* ------------------------------------------------------------------ helpers */

static ServerConfig make_config(int slots = 2)
{
    return ServerConfig(4242, 10, 20, {"alpha", "beta"}, slots, 100);
}

static Client make_client(int &out_peer)
{
    int sv[2];
    cr_assert_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
    fcntl(sv[0], F_SETFL, fcntl(sv[0], F_GETFL) | O_NONBLOCK);
    fcntl(sv[1], F_SETFL, fcntl(sv[1], F_GETFL) | O_NONBLOCK);
    out_peer = sv[1];
    return Client{FileDescriptor{sv[0]}};
}

static std::string drain_sent(Client &client, int peer_fd)
{
    client.buffer().on_writable();
    char buf[256] = {};
    const auto n = read(peer_fd, buf, sizeof(buf) - 1);
    return (n > 0) ? std::string(buf, static_cast<std::size_t>(n)) : std::string{};
}

/* --------------------------------------------------------- HandshakeHandler */

Test(handshake_handler, graphic_promotes_to_gui)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "GRAPHIC");
    cr_assert_eq(res, HandshakeResult::PROMOTED);
    cr_assert_eq(client.state(), ClientState::GUI);
    close(peer);
}

Test(handshake_handler, graphic_queues_no_extra_bytes)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    (void)h.handle(client, "GRAPHIC");
    const std::string sent = drain_sent(client, peer);
    cr_assert(sent.empty());
    close(peer);
}

Test(handshake_handler, valid_team_promotes_to_ai)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "alpha");
    cr_assert_eq(res, HandshakeResult::PROMOTED);
    cr_assert_eq(client.state(), ClientState::AI);
    cr_assert_eq(client.aiData().teamName, "alpha");
    close(peer);
}

Test(handshake_handler, valid_team_queues_free_slots_and_dimensions)
{
    auto config = make_config(3);
    TeamRegistry teams({"alpha"}, 3);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    (void)h.handle(client, "alpha");
    const std::string sent = drain_sent(client, peer);
    cr_assert_str_eq(sent.c_str(), "2\n10 20\n");
    close(peer);
}

Test(handshake_handler, unknown_team_drops_with_ko)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "ghost");
    cr_assert_eq(res, HandshakeResult::DROP);
    const std::string sent = drain_sent(client, peer);
    cr_assert_str_eq(sent.c_str(), "ko\n");
    close(peer);
}

Test(handshake_handler, full_team_drops_with_ko)
{
    auto config = make_config(1);
    TeamRegistry teams({"alpha"}, 1);
    HandshakeHandler h(config, teams);

    int peer1 = -1;
    auto client1 = make_client(peer1);
    (void)h.handle(client1, "alpha");
    close(peer1);

    int peer2 = -1;
    auto client2 = make_client(peer2);
    const auto res = h.handle(client2, "alpha");
    cr_assert_eq(res, HandshakeResult::DROP);
    const std::string sent = drain_sent(client2, peer2);
    cr_assert_str_eq(sent.c_str(), "ko\n");
    close(peer2);
}

Test(handshake_handler, empty_line_drops_with_ko)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "");
    cr_assert_eq(res, HandshakeResult::DROP);
    const std::string sent = drain_sent(client, peer);
    cr_assert_str_eq(sent.c_str(), "ko\n");
    close(peer);
}

Test(handshake_handler, whitespace_only_line_drops_with_ko)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "   \t");
    cr_assert_eq(res, HandshakeResult::DROP);
    close(peer);
}

Test(handshake_handler, leading_trailing_whitespace_tolerated_for_graphic)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, "  GRAPHIC  ");
    cr_assert_eq(res, HandshakeResult::PROMOTED);
    cr_assert_eq(client.state(), ClientState::GUI);
    close(peer);
}

Test(handshake_handler, leading_trailing_whitespace_tolerated_for_team)
{
    auto config = make_config();
    TeamRegistry teams({"alpha"}, 2);
    HandshakeHandler h(config, teams);
    int peer = -1;
    auto client = make_client(peer);

    const auto res = h.handle(client, " alpha ");
    cr_assert_eq(res, HandshakeResult::PROMOTED);
    cr_assert_eq(client.state(), ClientState::AI);
    close(peer);
}
