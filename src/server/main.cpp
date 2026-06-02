/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** zappy_server entry point: parse the CLI, then boot the server
*/

#include <unistd.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "Constants.hpp"
#include "server/network/Listener.hpp"
#include "server/network/NetworkError.hpp"
#include "server/network/PollLoop.hpp"
#include "server/posix/FileDescriptor.hpp"

/// @brief Read and log raw bytes from a client; drop it on EOF/error.
static void handle_client_data(int client_fd,
    zappy::server::network::PollLoop &poll,
    std::unordered_map<int, zappy::server::posix::FileDescriptor> &clients)
{
    std::array<char, zappy::server::READ_BUFFER_SIZE> buffer = {};
    const ssize_t count = ::read(client_fd, buffer.data(), buffer.size());

    if (count <= 0) {
        std::cout << "client " << client_fd << " disconnected\n";
        poll.unregister_fd(client_fd);
        clients.erase(client_fd);
        return;
    }
    std::cout << "received " << count << " byte(s) from client " << client_fd
              << "\n";
}

/// @brief Accept every pending connection, log it and watch it for data.
static void handle_new_connections(
    const zappy::server::network::Listener &listener,
    zappy::server::network::PollLoop &poll,
    std::unordered_map<int, zappy::server::posix::FileDescriptor> &clients)
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
        clients.emplace(client_fd, std::move(*client));
        poll.register_fd(client_fd, POLLIN, [client_fd, &poll, &clients](short) {
            handle_client_data(client_fd, poll, clients);
        });
    }
}

int main(int argc, char **argv)
{
    // TODO: use ServerConfig::port() from the CLI parser (ZAP-13) once merged.
    const std::uint16_t port = 4242;

    try {
        zappy::server::network::Listener listener(port);
        zappy::server::network::PollLoop poll;
        std::unordered_map<int, zappy::server::posix::FileDescriptor> clients;

        std::cout << "zappy_server listening on port " << listener.port()
                  << "\n";
        poll.register_fd(listener.fd(), POLLIN, [&](short) {
            handle_new_connections(listener, poll, clients);
        });
        poll.run();
        std::cout << "zappy_server shutting down\n";
    } catch (const zappy::server::network::NetworkError &error) {
        std::cerr << "Error: " << error.what() << "\n";
        return zappy::server::FAILURE;
    }
    return zappy::server::SUCCESS;
}
