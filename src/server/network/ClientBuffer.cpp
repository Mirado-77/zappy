/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** ClientBuffer implementation
*/

#include "Constants.hpp"
#include "server/network/ClientBuffer.hpp"

#include <array>
#include <cerrno>
#include <cstddef>
#include <string_view>
#include <unistd.h>

zappy::server::network::ClientBuffer::ClientBuffer(posix::FileDescriptor fd)
    : _fd(std::move(fd)), _write_offset(0)
{}

int zappy::server::network::ClientBuffer::fd() const noexcept
{
    return _fd.get();
}

zappy::server::network::ReadResult
zappy::server::network::ClientBuffer::_append_and_check(std::string_view data)
{
    ReadResult result = ReadResult::OK;

    _read_buffer.append(data);
    if (_read_buffer.size() > zappy::server::MAX_READ_BUF)
        result = ReadResult::ERROR;
    return result;
}

zappy::server::network::ReadResult zappy::server::network::ClientBuffer::on_readable()
{
    std::array<char, zappy::server::READ_CHUNK_SIZE> chunk = {};
    ReadResult result = ReadResult::OK;
    bool running = true;

    while (running && result == ReadResult::OK) {
        const ssize_t n = ::read(_fd.get(), chunk.data(), chunk.size());
        if (n == 0)
            result = ReadResult::PEER_CLOSED;
        else if (n > 0)
            result = _append_and_check(std::string_view(chunk.data(),
                static_cast<std::size_t>(n)));
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            running = false;
        else if (errno != EINTR)
            result = ReadResult::ERROR;
    }
    if (result == ReadResult::OK)
        _extract_lines();
    return result;
}

void zappy::server::network::ClientBuffer::_extract_lines()
{
    std::size_t pos = 0;
    std::size_t nl = _read_buffer.find('\n', pos);

    while (nl != std::string::npos) {
        std::string line = _read_buffer.substr(pos, nl - pos);
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        _ready_messages.push(std::move(line));
        pos = nl + 1;
        nl = _read_buffer.find('\n', pos);
    }
    _read_buffer.erase(0, pos);
}

void zappy::server::network::ClientBuffer::_load_next_message()
{
    if (_write_buffer.empty() && !_outgoing_messages.empty()) {
        _write_buffer = std::move(_outgoing_messages.front());
        _outgoing_messages.pop_front();
        _write_offset = 0;
    }
}

void zappy::server::network::ClientBuffer::_advance_write_buffer()
{
    _write_buffer.clear();
    _write_offset = 0;
    if (!_outgoing_messages.empty()) {
        _write_buffer = std::move(_outgoing_messages.front());
        _outgoing_messages.pop_front();
    }
}

bool zappy::server::network::ClientBuffer::_handle_write_result(
    std::ptrdiff_t n, WriteResult &result)
{
    bool done = false;

    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        result = WriteResult::OK;
        done = true;
    } else if (n < 0 && errno != EINTR) {
        result = WriteResult::ERROR;
        done = true;
    } else if (n >= 0) {
        _write_offset += static_cast<std::size_t>(n);
        if (_write_offset >= _write_buffer.size())
            _advance_write_buffer();
    }
    return done;
}

zappy::server::network::WriteResult zappy::server::network::ClientBuffer::on_writable()
{
    WriteResult result = WriteResult::ALL_SENT;
    bool done = false;

    _load_next_message();
    while (!_write_buffer.empty() && !done) {
        const ssize_t n = ::write(_fd.get(),
            _write_buffer.data() + _write_offset,
            _write_buffer.size() - _write_offset);
        done = _handle_write_result(static_cast<std::ptrdiff_t>(n), result);
    }
    return result;
}

void zappy::server::network::ClientBuffer::queue_message(std::string msg)
{
    if (msg.empty() || msg.back() != '\n')
        msg += '\n';
    _outgoing_messages.push_back(std::move(msg));
}

bool zappy::server::network::ClientBuffer::has_pending_write() const noexcept
{
    return !_write_buffer.empty() || !_outgoing_messages.empty();
}

std::optional<std::string> zappy::server::network::ClientBuffer::pop_ready_message()
{
    std::optional<std::string> result = std::nullopt;

    if (!_ready_messages.empty()) {
        result = std::move(_ready_messages.front());
        _ready_messages.pop();
    }
    return result;
}

bool zappy::server::network::ClientBuffer::has_ready_messages() const noexcept
{
    return !_ready_messages.empty();
}
