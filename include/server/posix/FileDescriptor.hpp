/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** RAII owner of a POSIX file descriptor
*/

#ifndef POSIX_FILEDESCRIPTOR_HPP_
    #define POSIX_FILEDESCRIPTOR_HPP_

namespace zappy::server::posix {

/// @brief Move-only RAII owner of a POSIX file descriptor.
class FileDescriptor {
public:
    /// @brief Build an invalid descriptor (-1).
    FileDescriptor() noexcept;

    /// @brief Take ownership of @p fd.
    explicit FileDescriptor(int fd) noexcept;

    /// @brief Close the owned descriptor.
    ~FileDescriptor();

    FileDescriptor(const FileDescriptor &) = delete;
    FileDescriptor &operator=(const FileDescriptor &) = delete;

    /// @brief Steal the descriptor from @p other.
    FileDescriptor(FileDescriptor &&other) noexcept;

    /// @brief Close the current descriptor, then steal @p other's.
    FileDescriptor &operator=(FileDescriptor &&other) noexcept;

    /// @return The raw descriptor without releasing ownership.
    [[nodiscard]] int get() const noexcept;

    /// @return The raw descriptor; ownership is transferred to the caller.
    [[nodiscard]] int release() noexcept;

    /// @brief Close the descriptor; idempotent.
    void close() noexcept;

    /// @return True if the descriptor is not -1.
    [[nodiscard]] bool is_valid() const noexcept;

    /// @return Same as is_valid().
    explicit operator bool() const noexcept;

private:
    int _fd;
};

}

#endif /* !POSIX_FILEDESCRIPTOR_HPP_ */
