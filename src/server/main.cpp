/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** zappy_server entry point: parse the CLI, then boot the server
*/

#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "server/Constants.hpp"
#include "server/cli/CliParseError.hpp"
#include "server/cli/CliParser.hpp"
#include "server/network/ClientBuffer.hpp"
#include "server/network/Listener.hpp"
#include "server/network/NetworkError.hpp"
#include "server/network/PollLoop.hpp"

/**
 * @brief Unregister @p client_fd from the poll loop and drop its buffer.
 *
 * @param client_fd file descriptor of the client to drop
 * @param poll the poll loop to unregister from
 * @param clients the client buffer map to erase from
 */
static void drop_client(int client_fd,
    zappy::server::network::PollLoop &poll,
    std::unordered_map<int, zappy::server::network::ClientBuffer> &clients)
{
    poll.unregister_fd(client_fd);
    clients.erase(client_fd);
}

/**
 * @brief Handle the events flagged on a client fd: read, echo, write, retune poll mask.
 *
 * @param client_fd file descriptor of the client
 * @param revents bitmask of events returned by poll for this fd
 * @param poll the poll loop (used to adjust the watched events)
 * @param clients the client buffer map
 */
static void handle_client_events(int client_fd, short revents,
    zappy::server::network::PollLoop &poll,
    std::unordered_map<int, zappy::server::network::ClientBuffer> &clients)
{
    auto it = clients.find(client_fd);
    if (it == clients.end())
        return;

    if (revents & POLLIN) {
        const auto result = it->second.on_readable();
        if (result != zappy::server::network::ReadResult::OK) {
            std::cerr << "client " << client_fd << " disconnected\n";
            drop_client(client_fd, poll, clients);
            return;
        }
        while (it->second.has_ready_messages()) {
            const auto msg = it->second.pop_ready_message();
            std::cerr << "[fd=" << client_fd << "] " << *msg << "\n";
            // TODO(ZAP-dispatcher): forward to Dispatcher instead of echoing
            it->second.queue_message(*msg);
        }
    }

    if (revents & POLLOUT) {
        const auto result = it->second.on_writable();
        if (result == zappy::server::network::WriteResult::ERROR) {
            std::cerr << "client " << client_fd << " write error\n";
            drop_client(client_fd, poll, clients);
            return;
        }
    }

    const short events = static_cast<short>(
        POLLIN | (it->second.has_pending_write() ? POLLOUT : 0));
    poll.modify_events(client_fd, events);
}

/**
 * @brief Accept every pending connection on @p listener and register them in @p poll.
 *
 * @param listener the listening socket to drain
 * @param poll the poll loop to register the new clients into
 * @param clients the client buffer map to populate
 */
static void handle_new_connections(
    const zappy::server::network::Listener &listener,
    zappy::server::network::PollLoop &poll,
    std::unordered_map<int, zappy::server::network::ClientBuffer> &clients)
{
    while (true) {
        std::optional<zappy::server::posix::FileDescriptor> client =
            listener.accept_connection();
        if (!client)
            break;

        const int client_fd = client->get();
        const zappy::server::posix::Address peer =
            zappy::server::network::Listener::peer_address(*client);

        std::cout << "client connected from " << peer.ip() << ":"
                  << peer.port() << "\n";
        clients.emplace(client_fd,
            zappy::server::network::ClientBuffer(std::move(*client)));
        poll.register_fd(client_fd, POLLIN,
            [client_fd, &poll, &clients](short revents) {
                handle_client_events(client_fd, revents, poll, clients);
            });
    }
}

/**
 * @brief zappy_server entry point: parse the CLI, then boot the server.
 *
 * @param argc number of command-line arguments
 * @param argv command-line argument vector
 * @return int SUCCESS on clean shutdown, FAILURE on CLI or network error
 */
int main(int argc, char **argv)
{
    try {
        const auto config = zappy::server::cli::CliParser{}.parse(argc, argv);

        if (!config)
            return zappy::server::SUCCESS;

        zappy::server::network::Listener listener(config->port());
        zappy::server::network::PollLoop poll;
        // TODO(ZAP-ClientRegistry): replace with ClientRegistry once implemented.
        std::unordered_map<int, zappy::server::network::ClientBuffer> clients;

        std::cout << "zappy_server listening on port " << listener.port()
                  << "\n";
        poll.register_fd(listener.fd(), POLLIN, [&](short) {
            handle_new_connections(listener, poll, clients);
        });
        poll.run();
        std::cout << "zappy_server shutting down\n";
    } catch (const zappy::server::cli::CliParseError &error) {
        std::cerr << "Error: " << error.what() << "\n";
        std::cerr << zappy::server::cli::CliParser::usage();
        return zappy::server::FAILURE;
    } catch (const zappy::server::network::NetworkError &error) {
        std::cerr << "Error: " << error.what() << "\n";
        return zappy::server::FAILURE;
    }
    return zappy::server::SUCCESS;
}
