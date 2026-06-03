/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Implementation of the POSIX file descriptor RAII owner
*/

#include "server/posix/FileDescriptor.hpp"
#include "server/Constants.hpp"
#include <unistd.h>

#include <utility>


/**
 * @brief Build an invalid descriptor.
 */
zappy::server::posix::FileDescriptor::FileDescriptor() noexcept
    : _fd(zappy::server::INVALID_FD)
{
}

/**
 * @brief Take ownership of @p fd.
 *
 * @param fd raw file descriptor to wrap (INVALID_FD for an empty descriptor)
 */
zappy::server::posix::FileDescriptor::FileDescriptor(int fd) noexcept : _fd(fd)
{
}

/**
 * @brief Close the owned descriptor.
 */
zappy::server::posix::FileDescriptor::~FileDescriptor()
{
    close();
}

/**
 * @brief Steal the descriptor from @p other.
 *
 * @param other source descriptor, left in an invalid state
 */
zappy::server::posix::FileDescriptor::FileDescriptor(
    FileDescriptor &&other) noexcept
    : _fd(other.release())
{
}

/**
 * @brief Close the current descriptor, then steal @p other's.
 *
 * @param other source descriptor, left in an invalid state
 * @return FileDescriptor& reference to this
 */
zappy::server::posix::FileDescriptor &
zappy::server::posix::FileDescriptor::operator=(FileDescriptor &&other) noexcept
{
    if (this != &other) {
        close();
        _fd = other.release();
    }
    return *this;
}

/**
 * @brief Return the raw descriptor without releasing ownership.
 *
 * @return int the raw file descriptor, or INVALID_FD if empty
 */
int zappy::server::posix::FileDescriptor::get() const noexcept
{
    return _fd;
}

/**
 * @brief Release ownership and return the raw descriptor.
 *
 * @return int the raw file descriptor; caller is now responsible for closing it
 */
int zappy::server::posix::FileDescriptor::release() noexcept
{
    return std::exchange(_fd, zappy::server::INVALID_FD);
}

/**
 * @brief Close the descriptor if open; idempotent.
 */
void zappy::server::posix::FileDescriptor::close() noexcept
{
    if (_fd != zappy::server::INVALID_FD) {
        ::close(_fd);
        _fd = zappy::server::INVALID_FD;
    }
}

/**
 * @brief Tell whether the descriptor is valid.
 *
 * @return true if the descriptor is open
 * @return false if the descriptor is INVALID_FD
 */
bool zappy::server::posix::FileDescriptor::is_valid() const noexcept
{
    return _fd != zappy::server::INVALID_FD;
}

/**
 * @brief Same as is_valid().
 *
 * @return true if the descriptor is open
 * @return false if the descriptor is INVALID_FD
 */
zappy::server::posix::FileDescriptor::operator bool() const noexcept
{
    return is_valid();
}
