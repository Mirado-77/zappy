/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** TCP listening socket wrapped in RAII, ready for poll
*/

#ifndef NETWORK_LISTENER_HPP_
    #define NETWORK_LISTENER_HPP_

    #include <cstdint>
    #include <optional>

    #include "server/posix/Address.hpp"
    #include "server/posix/FileDescriptor.hpp"

namespace zappy::server::network {

/// @brief Move-only non-blocking TCP listener bound to INADDR_ANY.
class Listener {
public:
    /// @brief Create, bind and listen on @p port; throws NetworkError on failure.
    explicit Listener(std::uint16_t port, int backlog = 128);

    Listener(const Listener &) = delete;
    Listener &operator=(const Listener &) = delete;
    Listener(Listener &&) noexcept = default;
    Listener &operator=(Listener &&) noexcept = default;

    /// @brief The raw listening descriptor for poll registration.
    [[nodiscard]] int fd() const noexcept;

    /// @brief The port actually bound (resolved when constructed with port 0).
    [[nodiscard]] std::uint16_t port() const noexcept;

    /// @brief Accept one pending connection; nullopt on EAGAIN, throws otherwise.
    [[nodiscard]] std::optional<posix::FileDescriptor> accept_connection() const;

    /// @brief The peer address of a connected socket.
    [[nodiscard]] static posix::Address peer_address(
        const posix::FileDescriptor &client);

private:
    posix::FileDescriptor _socket;
    std::uint16_t _port;
};

} // namespace zappy::server::network

#endif /* !NETWORK_LISTENER_HPP_ */
