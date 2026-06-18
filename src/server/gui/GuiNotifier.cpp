/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GuiNotifier implementation
*/

#include <utility>
#include "server/gui/GuiNotifier.hpp"

/**
 * @brief Construct a new Gui Notifier object.
 *
 * @param clients reference to the active client registry
 * @param world reference to the game world
 * @param retunePollMask callback to request adjustment of poll loop mask
 */
zappy::server::gui::GuiNotifier::GuiNotifier(
    client::ClientRegistry &clients,
    const game::World &world,
    std::function<void(int fd)> retunePollMask) noexcept
    : _clients(clients),
      _lineBuilder(world),
      _retunePollMask(std::move(retunePollMask))
{
}

/**
 * @brief Queue a message line to all GUI and GUI_ADMIN clients.
 *
 * @param line message text (without trailing newline)
 */
void zappy::server::gui::GuiNotifier::broadcast(const std::string &line)
{
    auto callback = [&line, this](client::Client &client) {
        client.buffer().queue_message(line);
        _retunePollMask(client.fd());
    };

    _clients.forEachInState(client::ClientState::GUI, callback);
    _clients.forEachInState(client::ClientState::GUI_ADMIN, callback);
}

/**
 * @brief Translate onTileChanged world events into GUI bct updates.
 *
 * @param pos coordinate of the affected tile
 */
void zappy::server::gui::GuiNotifier::onTileChanged(game::Position pos)
{
    broadcast(_lineBuilder.bct(pos.x(), pos.y()));
}
