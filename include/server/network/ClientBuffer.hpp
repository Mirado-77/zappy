/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Per-fd buffering layer between PollLoop and the dispatcher
*/

#ifndef NETWORK_CLIENTBUFFER_HPP_
    #define NETWORK_CLIENTBUFFER_HPP_

    #include <cstddef>
    #include <deque>
    #include <optional>
    #include <queue>
    #include <string>
    #include <string_view>

    #include "server/network/NetworkError.hpp"
    #include "server/posix/FileDescriptor.hpp"

namespace zappy::server::network {

enum class ReadResult  { OK, PEER_CLOSED, ERROR };
enum class WriteResult { OK, ERROR, ALL_SENT };

/// @brief Thrown on programmer errors (e.g. calling on_readable after PEER_CLOSED).
class BufferError : public NetworkError {
public:
    explicit BufferError(const std::string &reason) : NetworkError(reason) {}
};

/// @brief Move-only per-client I/O buffer: accumulates reads, drains a write queue.
class ClientBuffer {
public:
    /// @brief Take ownership of @p fd; @p fd must already be O_NONBLOCK.
    explicit ClientBuffer(posix::FileDescriptor fd);

    ClientBuffer(const ClientBuffer &) = delete;
    ClientBuffer &operator=(const ClientBuffer &) = delete;
    ClientBuffer(ClientBuffer &&) noexcept = default;
    ClientBuffer &operator=(ClientBuffer &&) noexcept = default;

    /// @return Raw descriptor for poll registration.
    [[nodiscard]] int fd() const noexcept;

    /// @brief Read available bytes; extract newline-delimited lines into ready_messages.
    ReadResult on_readable();

    /// @brief Drain the outgoing queue progressively; call when POLLOUT fires.
    WriteResult on_writable();

    /// @brief Enqueue @p msg for sending; appends '\\n' if absent.
    void queue_message(std::string msg);

    /// @return True when POLLOUT should be registered.
    [[nodiscard]] bool has_pending_write() const noexcept;

    /// @return Next fully-received line, or nullopt.
    [[nodiscard]] std::optional<std::string> pop_ready_message();

    [[nodiscard]] bool has_ready_messages() const noexcept;

private:
    ReadResult _append_and_check(std::string_view data);
    void _extract_lines();
    void _load_next_message();
    bool _handle_write_result(std::ptrdiff_t n, WriteResult &result);
    void _advance_write_buffer();

    posix::FileDescriptor _fd;
    std::string _read_buffer;
    std::deque<std::string> _outgoing_messages;
    std::string _write_buffer;
    std::size_t _write_offset;
    std::queue<std::string> _ready_messages;
};

} // namespace zappy::server::network

#endif /* !NETWORK_CLIENTBUFFER_HPP_ */
