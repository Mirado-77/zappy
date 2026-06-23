/*
** EPITECH PROJECT, 2026
** Gui.cpp
** File description:
** Server implementation
*/

#include "gui/Gui.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/*
#include "server/game/EggSpawner.hpp"
*/

/**
 * @brief Initialise all members from the validated configuration.
 *
 * Member order matches the declaration order in Server.hpp:
 * _config → _featureFlags → _listener → _poll → _world → _resourceSpawner →
 * _clients → _clock → _scheduler → _refillScheduler → _foodScheduler →
 * _aiDispatcher → _handshake.
 *
 * @param config validated CLI configuration
 */
zappy::gui::Gui::Gui(const ipc::ServerInfo &server_info)
    : _server_info(server_info)
{
}

/**
 * @brief conect to the server
 */
void zappy::gui::Gui::connectServer()
{
/*
    std::optional<posix::FileDescriptor> conn =
        _listener.accept_connection();
    while (conn) {
        const int cfd = conn->get();
        const posix::Address peer =
            net::Listener::peer_address(*conn);
        client::Client &c = _clients.add(std::move(*conn));
        c.buffer().queue_message("WELCOME");
        std::cout << "client connected from "
                  << peer.ip() << ":" << peer.port() << "\n";
        _poll.register_fd(cfd, POLLIN, [this, cfd](short revents) {
            handleClientEvents(cfd, revents);
        });
        retunePollMask(cfd);
        conn = _listener.accept_connection();
    }
*/
}

/**
 * @brief pull events from server and dispatch them
 */
void zappy::gui::Gui::handleServerEvents()
{
/*
    bool dropped = !_clients.contains(clientFd);
    if (!dropped && (revents & POLLIN))
        dropped = handlePollin(clientFd);
    if (!dropped && (revents & POLLOUT))
        dropped = handlePollout(clientFd);
    if (!dropped)
        retunePollMask(clientFd);
    if (!dropped && _clients.get(clientFd).isMarkedForDrop() &&
        !_clients.get(clientFd).buffer().has_pending_write())
        dropClient(clientFd);
*/
}

/**
 * @brief Broadcast a message to the server.
 */
void zappy::gui::Gui::Broadcast()
{
/*
    char buf[1024];
    const ssize_t n = read(0, buf, sizeof(buf) - 1);

    if (n <= 0) {
        _poll.unregister_fd(0);
        return;
    }
    buf[n] = '\0';
    std::string line(buf);
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
        line.pop_back();
    if (!line.empty())
        _guiNotifier.broadcastMessage(line);
*/
}

zappy::gui::Gui::~Gui()
{
    ; // destroy the window
}

void zappy::gui::Gui::run()
{
    ;
}

void zappy::gui::Gui::pullServerInfo()
{
    ;
}

void zappy::gui::Gui::init()
{
    ;
}

void zappy::gui::Gui::update()
{
    ;
}
