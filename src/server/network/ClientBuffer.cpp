/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** ClientBuffer implementation
*/

#include "server/network/ClientBuffer.hpp"

#include <cerrno>
#include <unistd.h>

namespace zappy::server::network {

static constexpr std::size_t READ_CHUNK   = 4096;
static constexpr std::size_t MAX_READ_BUF = 1024U * 1024U;

ClientBuffer::ClientBuffer(posix::FileDescriptor fd)
    : _fd(std::move(fd)), _write_offset(0)
{}

int ClientBuffer::fd() const noexcept
{
    return _fd.get();
}

ReadResult ClientBuffer::on_readable()
{
    char chunk[READ_CHUNK];

    while (true) {
        const ssize_t n = ::read(_fd.get(), chunk, sizeof(chunk));
        if (n == 0)
            return ReadResult::PEER_CLOSED;
        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            return ReadResult::ERROR;
        }
        _read_buffer.append(chunk, static_cast<std::size_t>(n));
        if (_read_buffer.size() > MAX_READ_BUF)
            return ReadResult::ERROR;
    }
    _extract_lines();
    return ReadResult::OK;
}

WriteResult ClientBuffer::on_writable()
{
    if (_write_buffer.empty() && !_outgoing_messages.empty()) {
        _write_buffer = std::move(_outgoing_messages.front());
        _outgoing_messages.pop_front();
        _write_offset = 0;
    }

    while (!_write_buffer.empty()) {
        const char        *data      = _write_buffer.data() + _write_offset;
        const std::size_t  remaining = _write_buffer.size() - _write_offset;
        const ssize_t      n         = ::write(_fd.get(), data, remaining);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return WriteResult::OK;
            return WriteResult::ERROR;
        }
        _write_offset += static_cast<std::size_t>(n);
        if (_write_offset >= _write_buffer.size()) {
            _write_buffer.clear();
            _write_offset = 0;
            if (!_outgoing_messages.empty()) {
                _write_buffer = std::move(_outgoing_messages.front());
                _outgoing_messages.pop_front();
            }
        }
    }
    return WriteResult::ALL_SENT;
}

void ClientBuffer::queue_message(std::string msg)
{
    if (msg.empty() || msg.back() != '\n')
        msg += '\n';
    _outgoing_messages.push_back(std::move(msg));
}

bool ClientBuffer::has_pending_write() const noexcept
{
    return !_write_buffer.empty() || !_outgoing_messages.empty();
}

std::optional<std::string> ClientBuffer::pop_ready_message()
{
    if (_ready_messages.empty())
        return std::nullopt;
    std::string msg = std::move(_ready_messages.front());
    _ready_messages.pop();
    return msg;
}

bool ClientBuffer::has_ready_messages() const noexcept
{
    return !_ready_messages.empty();
}

void ClientBuffer::_extract_lines()
{
    std::size_t pos = 0;
    std::size_t nl  = 0;

    while ((nl = _read_buffer.find('\n', pos)) != std::string::npos) {
        std::string line = _read_buffer.substr(pos, nl - pos);
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        _ready_messages.push(std::move(line));
        pos = nl + 1;
    }
    _read_buffer.erase(0, pos);
}

} // namespace zappy::server::network
