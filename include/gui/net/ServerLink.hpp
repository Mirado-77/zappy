/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GUI <-> server TCP link, non-blocking
*/

#ifndef GUI_NET_SERVERLINK_HPP_
    #define GUI_NET_SERVERLINK_HPP_

    #include <cstdint>
    #include <memory>
    #include <optional>
    #include <stdexcept>
    #include <string>

    #include "net/ClientBuffer.hpp"
    #include "protocol/gui/GuiCommandParser.hpp"
    #include "protocol/gui/ParsedGuiCommand.hpp"

namespace zappy::gui::net {

class ServerLinkError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ServerLink {
public:
    ServerLink(const std::string &host, std::uint16_t port);

    ServerLink(const ServerLink &) = delete;
    ServerLink &operator=(const ServerLink &) = delete;

    void send(std::string line);

    void poll_once(int timeout_ms);

    [[nodiscard]] std::optional<zappy::protocol::gui::ParsedGuiCommand>
        next_message();

    [[nodiscard]] bool is_connected() const noexcept;

private:
    enum class HandshakeState { WAIT_WELCOME, READY };

    void open_socket(const std::string &host, std::uint16_t port);
    void process_line(const std::string &line);

    std::unique_ptr<zappy::net::ClientBuffer> _buffer;
    HandshakeState _state;
    bool _connected;
    std::queue<zappy::protocol::gui::ParsedGuiCommand> _ready;
};

} // namespace zappy::gui::net

#endif /* !GUI_NET_SERVERLINK_HPP_ */