/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GuiNotifier class translating world events into GUI broadcasts
*/

#ifndef SERVER_GUI_GUINOTIFIER_HPP_
    #define SERVER_GUI_GUINOTIFIER_HPP_

    #include <functional>
    #include <string>
    #include "server/game/IWorldObserver.hpp"
    #include "server/client/ClientRegistry.hpp"
    #include "server/gui/GuiLineBuilder.hpp"

namespace zappy::server::gui {

/**
 * @brief Listens to World mutations as an IWorldObserver and broadcasts
 *        equivalent protocol notifications to all connected GUI clients.
 */
class GuiNotifier : public game::WorldObserverAdapter {
public:
    /**
     * @brief Construct a new Gui Notifier.
     *
     * @param clients reference to the active client registry
     * @param world reference to the game world
     * @param retunePollMask callback to request adjustment of poll loop mask
     */
    GuiNotifier(client::ClientRegistry &clients,
                const game::World &world,
                std::function<void(int fd)> retunePollMask) noexcept;

    GuiNotifier(const GuiNotifier &) = delete;
    GuiNotifier &operator=(const GuiNotifier &) = delete;
    GuiNotifier(GuiNotifier &&) noexcept = default;
    GuiNotifier &operator=(GuiNotifier &&) noexcept = default;

    ~GuiNotifier() override = default;

    /**
     * @brief Queue a message line to all GUI and GUI_ADMIN clients.
     *
     * @param line message text (without trailing newline)
     */
    void broadcast(const std::string &line);

    /**
     * @brief Triggered when a tile's resource layout is modified.
     *
     * @param pos coordinate of the affected tile
     */
    void onTileChanged(game::Position pos) override;

private:
    client::ClientRegistry &_clients;
    GuiLineBuilder _lineBuilder;
    std::function<void(int fd)> _retunePollMask;
};

} // namespace zappy::server::gui

#endif /* !SERVER_GUI_GUINOTIFIER_HPP_ */
