/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Unit tests for GuiNotifier class
*/

#include <criterion/criterion.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <cerrno>

#include "server/game/World.hpp"
#include "server/client/ClientRegistry.hpp"
#include "server/gui/GuiNotifier.hpp"

using zappy::server::game::World;
using zappy::server::game::Position;
using zappy::server::game::ResourceType;
using zappy::server::client::Client;
using zappy::server::client::ClientRegistry;
using zappy::server::client::ClientState;
using zappy::server::gui::GuiNotifier;
using zappy::posix::FileDescriptor;

struct NotifierFixture {
    World world{10, 10, {"team1"}, 2};
    ClientRegistry clients;
    std::vector<int> retunedFds;
    std::vector<int> peerFds;
    GuiNotifier notifier{clients, world, [this](int fd) {
        retunedFds.push_back(fd);
    }};

    int addClient(ClientState state)
    {
        int sv[2];
        cr_assert_eq(socketpair(AF_UNIX, SOCK_STREAM, 0, sv), 0);
        fcntl(sv[0], F_SETFL, fcntl(sv[0], F_GETFL) | O_NONBLOCK);
        fcntl(sv[1], F_SETFL, fcntl(sv[1], F_GETFL) | O_NONBLOCK);
        peerFds.push_back(sv[1]);
        Client &c = clients.add(FileDescriptor{sv[0]});
        if (state != ClientState::HANDSHAKE) {
            c.promote(state);
        }
        return sv[1];
    }

    ~NotifierFixture()
    {
        for (int fd : peerFds) {
            close(fd);
        }
    }
};

Test(gui_notifier, test_broadcast_to_gui_clients_only)
{
    NotifierFixture f;
    int guiPeer = f.addClient(ClientState::GUI);
    int adminPeer = f.addClient(ClientState::GUI_ADMIN);
    int aiPeer = f.addClient(ClientState::AI);
    int handshakePeer = f.addClient(ClientState::HANDSHAKE);

    f.notifier.broadcast("test message");

    cr_assert_eq(f.retunedFds.size(), 2U);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[128] = {0};
    cr_assert_gt(read(guiPeer, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "test message\n");

    std::memset(buf, 0, sizeof(buf));
    cr_assert_gt(read(adminPeer, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "test message\n");

    std::memset(buf, 0, sizeof(buf));
    cr_assert_eq(read(aiPeer, buf, sizeof(buf) - 1), -1);
    cr_assert_eq(errno, EAGAIN);

    cr_assert_eq(read(handshakePeer, buf, sizeof(buf) - 1), -1);
    cr_assert_eq(errno, EAGAIN);
}

Test(gui_notifier, test_broadcast_no_gui_clients)
{
    NotifierFixture f;
    int aiPeer = f.addClient(ClientState::AI);
    int handshakePeer = f.addClient(ClientState::HANDSHAKE);

    f.notifier.broadcast("test message");

    cr_assert_eq(f.retunedFds.size(), 0U);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[128] = {0};
    cr_assert_eq(read(aiPeer, buf, sizeof(buf) - 1), -1);
    cr_assert_eq(errno, EAGAIN);

    cr_assert_eq(read(handshakePeer, buf, sizeof(buf) - 1), -1);
    cr_assert_eq(errno, EAGAIN);
}

Test(gui_notifier, test_broadcast_multiple_gui_clients)
{
    NotifierFixture f;
    int guiPeer1 = f.addClient(ClientState::GUI);
    int guiPeer2 = f.addClient(ClientState::GUI);
    int adminPeer = f.addClient(ClientState::GUI_ADMIN);

    f.notifier.broadcast("msg");

    cr_assert_eq(f.retunedFds.size(), 3U);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[128] = {0};
    cr_assert_gt(read(guiPeer1, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "msg\n");

    std::memset(buf, 0, sizeof(buf));
    cr_assert_gt(read(guiPeer2, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "msg\n");

    std::memset(buf, 0, sizeof(buf));
    cr_assert_gt(read(adminPeer, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "msg\n");
}

Test(gui_notifier, test_on_tile_changed_fires_bct)
{
    NotifierFixture f;
    f.world.addObserver(f.notifier);
    int guiPeer = f.addClient(ClientState::GUI);

    f.world.setTileResource(Position(0, 0), ResourceType::FOOD, 5);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[128] = {0};
    cr_assert_gt(read(guiPeer, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "bct 0 0 5 0 0 0 0 0 0\n");
}

Test(gui_notifier, test_multiple_mutations_multiple_broadcasts)
{
    NotifierFixture f;
    f.world.addObserver(f.notifier);
    int guiPeer = f.addClient(ClientState::GUI);

    f.world.setTileResource(Position(1, 2), ResourceType::LINEMATE, 3);
    f.world.setTileResource(Position(3, 4), ResourceType::DERAUMERE, 4);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[256] = {0};
    cr_assert_gt(read(guiPeer, buf, sizeof(buf) - 1), 0);
    std::string received(buf);
    cr_assert_neq(received.find("bct 1 2 0 3 0 0 0 0 0\n"), std::string::npos);
    cr_assert_neq(received.find("bct 3 4 0 0 4 0 0 0 0\n"), std::string::npos);
}

Test(gui_notifier, test_empty_buffer_state_before_broadcast)
{
    NotifierFixture f;
    f.world.addObserver(f.notifier);
    int guiPeer = f.addClient(ClientState::GUI);

    f.clients.forEach([](Client &c) {
        cr_assert_not(c.buffer().has_pending_write());
    });

    f.world.setTileResource(Position(0, 0), ResourceType::FOOD, 1);

    bool hasWrite = false;
    f.clients.forEach([&hasWrite](Client &c) {
        if (c.buffer().has_pending_write())
            hasWrite = true;
    });
    cr_assert(hasWrite);

    f.clients.forEach([](Client &c) {
        c.buffer().on_writable();
    });

    char buf[128] = {0};
    cr_assert_gt(read(guiPeer, buf, sizeof(buf) - 1), 0);
    cr_assert_str_eq(buf, "bct 0 0 1 0 0 0 0 0 0\n");
}
