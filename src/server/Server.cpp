/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Server implementation
*/

#include "server/Server.hpp"
#include <iostream>
#include <optional>

/**
 * @brief Initialise all members from the validated configuration.
 *
 * Member order matches the declaration order in Server.hpp:
 * _config → _listener → _poll → _teams → _clients → _handshake.
 *
 * @param config validated CLI configuration
 */
zappy::server::Server::Server(const cli::ServerConfig &config)
    : _config(config),
      _listener(_config.port()),
      _poll(),
      _teams(_config.teamNames(), _config.clientsPerTeam()),
      _clients(),
      _handshake(_config, _teams)
{}

/**
 * @brief Register the accept callback and enter the poll loop.
 */
void zappy::server::Server::run()
{
    std::cout << "zappy_server listening on port "
              << _listener.port() << "\n";
    _poll.register_fd(_listener.fd(), POLLIN, [this](short) {
        acceptPendingClients();
    });
    _poll.run();
    std::cout << "zappy_server shutting down\n";
}

/**
 * @brief Accept all pending connections, send WELCOME, and register each in the poll loop.
 */
void zappy::server::Server::acceptPendingClients()
{
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
}

/**
 * @brief Dispatch events for @p clientFd; skip if already removed from registry.
 *
 * @param clientFd file descriptor that received events
 * @param revents bitmask of events returned by poll
 */
void zappy::server::Server::handleClientEvents(
    int clientFd, short revents)
{
    bool dropped = !_clients.contains(clientFd);
    if (!dropped && (revents & POLLIN))
        dropped = handlePollin(clientFd);
    if (!dropped && (revents & POLLOUT))
        dropped = handlePollout(clientFd);
    if (!dropped)
        retunePollMask(clientFd);
}

/**
 * @brief Read available data, then dispatch ready messages; drop client on error or EOF.
 *
 * @param clientFd readable file descriptor
 * @return true if the client was dropped
 */
bool zappy::server::Server::handlePollin(int clientFd)
{
    const auto result =
        _clients.get(clientFd).buffer().on_readable();
    bool dropped = result != net::ReadResult::OK;
    if (dropped) {
        std::cerr << "client " << clientFd << " disconnected\n";
        dropClient(clientFd);
    }
    if (!dropped)
        dropped = dispatchReadyMessages(clientFd);
    return dropped;
}

/**
 * @brief Drain the write queue; drop client on write error.
 *
 * @param clientFd writable file descriptor
 * @return true if the client was dropped
 */
bool zappy::server::Server::handlePollout(int clientFd)
{
    const auto result =
        _clients.get(clientFd).buffer().on_writable();
    bool dropped = result == net::WriteResult::ERROR;
    if (dropped) {
        std::cerr << "client " << clientFd << " write error\n";
        dropClient(clientFd);
    }
    return dropped;
}

/**
 * @brief Release the team slot (if AI), unregister from poll, and erase from registry.
 *
 * @param clientFd file descriptor to drop
 */
void zappy::server::Server::dropClient(int clientFd)
{
    if (_clients.contains(clientFd) &&
        _clients.get(clientFd).state() == client::ClientState::AI)
        _teams.releaseSlot(_clients.get(clientFd).aiData().teamName);
    _poll.unregister_fd(clientFd);
    _clients.remove(clientFd);
}

/**
 * @brief Recompute POLLOUT need and update the poll mask for @p clientFd.
 *
 * @param clientFd file descriptor to retune
 */
void zappy::server::Server::retunePollMask(int clientFd)
{
    const bool wantWrite =
        _clients.get(clientFd).buffer().has_pending_write();
    const short events =
        static_cast<short>(POLLIN | (wantWrite ? POLLOUT : 0));
    _poll.modify_events(clientFd, events);
}

/**
 * @brief Route each ready message to the appropriate handler based on client state.
 *
 * For HANDSHAKE: delegates to HandshakeHandler; drops the client on DROP.
 * For other states: messages are silently consumed (real dispatchers are future work).
 *
 * @param clientFd file descriptor with ready messages
 * @return true if the client was dropped during dispatch
 */
bool zappy::server::Server::dispatchReadyMessages(int clientFd)
{
    client::Client &c = _clients.get(clientFd);
    bool dropped = false;
    while (!dropped && c.buffer().has_ready_messages()) {
        const auto msg = c.buffer().pop_ready_message();
        if (c.state() == client::ClientState::HANDSHAKE) {
            const auto res = _handshake.handle(c, *msg);
            dropped = (res == handshake::HandshakeResult::DROP);
            if (dropped)
                dropClient(clientFd);
        }
    }
    return dropped;
}
