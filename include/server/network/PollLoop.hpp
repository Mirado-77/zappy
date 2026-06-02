/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** poll()-based event loop with per-fd callbacks
*/

#ifndef NETWORK_POLLLOOP_HPP_
    #define NETWORK_POLLLOOP_HPP_

    #include <poll.h>

    #include <atomic>
    #include <functional>
    #include <unordered_map>
    #include <vector>

    #include "server/network/SignalHandler.hpp"
    #include "server/posix/FileDescriptor.hpp"

namespace zappy::server::network {

/// @brief Single-threaded poll() event loop dispatching to per-fd callbacks.
class PollLoop {
public:
    /// @brief Callback invoked with the ready revents of its descriptor.
    typedef std::function<void(short revents)> Callback;

    /// @brief Build the loop and register the SIGINT self-pipe.
    PollLoop();

    PollLoop(const PollLoop &) = delete;
    PollLoop &operator=(const PollLoop &) = delete;

    /// @brief Watch @p fd for @p events and dispatch to @p callback.
    void register_fd(int fd, short events, Callback callback);

    /// @brief Stop watching @p fd.
    void unregister_fd(int fd);

    /// @brief Change the watched events of an already registered @p fd.
    void modify_events(int fd, short events);

    /// @brief Set the poll timeout in ms (-1 blocks forever); used later by the Scheduler.
    void set_next_timeout(int ms) noexcept;

    /// @brief Run until stop() or SIGINT.
    void run();

    /// @brief Request the loop to stop and wake it up; thread/signal safe.
    void stop() noexcept;

private:
    /// @brief Dispatch the descriptors flagged ready by the last poll.
    void dispatch();

    /// @brief Drain the eventfd used to wake the loop.
    void drain_wake_event() noexcept;

    std::vector<pollfd> _pollfds;
    std::unordered_map<int, Callback> _callbacks;
    std::atomic<bool> _stop_requested;
    SignalHandler _signals;
    posix::FileDescriptor _wake_event;
    int _timeout_ms;
};

} // namespace zappy::server::network

#endif /* !NETWORK_POLLLOOP_HPP_ */
