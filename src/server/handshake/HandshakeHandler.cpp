/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** HandshakeHandler implementation
*/

#include <string>

#include "server/handshake/HandshakeHandler.hpp"

/**
 * @brief Bind the handler to the server configuration and team registry.
 *
 * @param config immutable server configuration
 * @param teams mutable team slot registry
 */
zappy::server::handshake::HandshakeHandler::HandshakeHandler(
    const cli::ServerConfig &config, game::TeamRegistry &teams)
    : _config(config), _teams(teams)
{}

/**
 * @brief Strip leading and trailing ASCII whitespace and carriage returns.
 *
 * @param s raw input
 * @return std::string trimmed copy
 */
std::string zappy::server::handshake::HandshakeHandler::trim(
    const std::string &s)
{
    const auto first = s.find_first_not_of(" \t\r");
    const auto last = s.find_last_not_of(" \t\r");
    return (first == std::string::npos) ? "" : s.substr(first, last - first + 1);
}

/**
 * @brief Promote @p client to GUI; no further bytes are queued here.
 *
 * @param client HANDSHAKE-state client
 * @return HandshakeResult always PROMOTED
 */
zappy::server::handshake::HandshakeResult
zappy::server::handshake::HandshakeHandler::promoteToGui(
    client::Client &client)
{
    client.promote(client::ClientState::GUI);
    return HandshakeResult::PROMOTED;
}

/**
 * @brief Attempt to promote @p client to AI for team @p name.
 *
 * Queues "ko" and returns DROP when the team is unknown or full.
 * Otherwise reserves a slot, promotes, and queues the two-line reply.
 *
 * @param client HANDSHAKE-state client
 * @param name trimmed team name
 * @return HandshakeResult PROMOTED or DROP
 */
zappy::server::handshake::HandshakeResult
zappy::server::handshake::HandshakeHandler::promoteToAi(
    client::Client &client, const std::string &name)
{
    HandshakeResult result = HandshakeResult::DROP;
    if (!_teams.contains(name) || !_teams.hasSlot(name)) {
        client.buffer().queue_message("ko");
    } else {
        _teams.reserveSlot(name);
        client.promote(client::ClientState::AI);
        client.aiData().teamName = name;
        client.buffer().queue_message(std::to_string(_teams.freeSlots(name)));
        client.buffer().queue_message(
            std::to_string(_config.width()) + " " +
            std::to_string(_config.height()));
        result = HandshakeResult::PROMOTED;
    }
    return result;
}

/**
 * @brief Route @p line to the appropriate promotion path.
 *
 * Trims whitespace first; an empty line is rejected immediately.
 *
 * @param client HANDSHAKE-state client
 * @param line raw line from the network (no trailing newline)
 * @return HandshakeResult PROMOTED or DROP
 */
zappy::server::handshake::HandshakeResult
zappy::server::handshake::HandshakeHandler::handle(
    client::Client &client, const std::string &line)
{
    const std::string name = trim(line);
    HandshakeResult result = HandshakeResult::DROP;
    if (name.empty())
        client.buffer().queue_message("ko");
    else if (name == "GRAPHIC")
        result = promoteToGui(client);
    else
        result = promoteToAi(client, name);
    return result;
}
