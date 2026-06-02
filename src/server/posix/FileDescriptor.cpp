/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Implementation of the POSIX file descriptor RAII owner
*/

#include "server/posix/FileDescriptor.hpp"
#include "Constants.hpp"
#include <unistd.h>

#include <utility>


/// @brief Build an invalid descriptor.
zappy::server::posix::FileDescriptor::FileDescriptor() noexcept
    : _fd(zappy::server::INVALID_FD)
{
}

/// @brief Take ownership of @p fd.
zappy::server::posix::FileDescriptor::FileDescriptor(int fd) noexcept : _fd(fd)
{
}

/// @brief Close the owned descriptor.
zappy::server::posix::FileDescriptor::~FileDescriptor()
{
    close();
}

/// @brief Steal the descriptor from @p other.
zappy::server::posix::FileDescriptor::FileDescriptor(
    FileDescriptor &&other) noexcept
    : _fd(other.release())
{
}

/// @brief Close the current descriptor, then steal @p other's.
zappy::server::posix::FileDescriptor &
zappy::server::posix::FileDescriptor::operator=(FileDescriptor &&other) noexcept
{
    if (this != &other) {
        close();
        _fd = other.release();
    }
    return *this;
}

/// @brief Return the raw descriptor without releasing ownership.
int zappy::server::posix::FileDescriptor::get() const noexcept
{
    return _fd;
}

/// @brief Release ownership and return the raw descriptor.
int zappy::server::posix::FileDescriptor::release() noexcept
{
    return std::exchange(_fd, zappy::server::INVALID_FD);
}

/// @brief Close the descriptor if open; idempotent.
void zappy::server::posix::FileDescriptor::close() noexcept
{
    if (_fd != zappy::server::INVALID_FD) {
        ::close(_fd);
        _fd = zappy::server::INVALID_FD;
    }
}

/// @brief Tell whether the descriptor is valid.
bool zappy::server::posix::FileDescriptor::is_valid() const noexcept
{
    return _fd != zappy::server::INVALID_FD;
}

/// @brief Same as is_valid().
zappy::server::posix::FileDescriptor::operator bool() const noexcept
{
    return is_valid();
}
