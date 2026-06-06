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
 * @brief Initialise listener on the configured port; leave poll and registry empty.
 *
 * @param config validated CLI configuration
 */
zappy::server::Server::Server(const cli::ServerConfig &config)
    : _listener(config.port()), _poll(), _clients()
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
 * @brief Accept all pending connections and register each in the poll loop.
 */
void zappy::server::Server::acceptPendingClients()
{
    std::optional<posix::FileDescriptor> conn =
        _listener.accept_connection();
    while (conn) {
        const int cfd = conn->get();
        const posix::Address peer =
            network::Listener::peer_address(*conn);
        _clients.add(std::move(*conn));
        std::cout << "client connected from "
                  << peer.ip() << ":" << peer.port() << "\n";
        _poll.register_fd(cfd, POLLIN, [this, cfd](short revents) {
            handleClientEvents(cfd, revents);
        });
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
 * @brief Read available data; echo complete lines; drop client on error or EOF.
 *
 * @param clientFd readable file descriptor
 * @return true if the client was dropped
 */
bool zappy::server::Server::handlePollin(int clientFd)
{
    const auto result =
        _clients.get(clientFd).buffer().on_readable();
    bool dropped = result != network::ReadResult::OK;
    if (dropped) {
        std::cerr << "client " << clientFd << " disconnected\n";
        dropClient(clientFd);
    }
    if (!dropped)
        echoReadyMessages(clientFd);
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
    bool dropped = result == network::WriteResult::ERROR;
    if (dropped) {
        std::cerr << "client " << clientFd << " write error\n";
        dropClient(clientFd);
    }
    return dropped;
}

/**
 * @brief Unregister @p clientFd from the poll loop and erase it from the registry.
 *
 * @param clientFd file descriptor to drop
 */
void zappy::server::Server::dropClient(int clientFd)
{
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
 * @brief Pop all ready lines from @p clientFd's buffer and queue each as a reply.
 *
 * @param clientFd file descriptor with ready messages
 */
void zappy::server::Server::echoReadyMessages(int clientFd)
{
    client::Client &c = _clients.get(clientFd);
    while (c.buffer().has_ready_messages()) {
        const auto msg = c.buffer().pop_ready_message();
        std::cerr << "[fd=" << clientFd << "] " << *msg << "\n";
        c.buffer().queue_message(*msg);
    }
}
