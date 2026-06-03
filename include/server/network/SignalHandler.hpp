/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** SIGINT delivery as a pollable file descriptor (signalfd), no global state
*/

#ifndef NETWORK_SIGNALHANDLER_HPP_
    #define NETWORK_SIGNALHANDLER_HPP_

    #include "server/posix/FileDescriptor.hpp"

namespace zappy::server::network {

/**
 * @brief Turns SIGINT into a readable descriptor via signalfd; no globals.
 */
class SignalHandler {
public:
    /**
     * @brief Block SIGINT and open its signalfd.
     */
    SignalHandler();

    SignalHandler(const SignalHandler &) = delete;
    SignalHandler &operator=(const SignalHandler &) = delete;
    SignalHandler(SignalHandler &&) noexcept = default;
    SignalHandler &operator=(SignalHandler &&) noexcept = default;

    /**
     * @brief The signalfd to register in the poll loop.
     *
     * @return int the signalfd file descriptor
     */
    [[nodiscard]] int read_fd() const noexcept;

    /**
     * @brief Drain pending signal notifications.
     */
    void consume() const noexcept;

private:
    posix::FileDescriptor _signal_fd;
};

} // namespace zappy::server::network

#endif /* !NETWORK_SIGNALHANDLER_HPP_ */
