/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GUI command dispatcher routing lines to stubs
*/

#ifndef SERVER_GUI_GUIDISPATCHER_HPP_
    #define SERVER_GUI_GUIDISPATCHER_HPP_

    #include <array>
    #include <functional>
    #include <string>
    #include <string_view>
    #include <vector>

    #include "server/game/World.hpp"
    #include "protocol/gui/GuiCommandParser.hpp"

namespace zappy::server::gui {

/**
 * @brief Parses incoming GUI command lines and
 * executes the corresponding stub.
 */
class GuiDispatcher {
public:
    /**
     * @brief Wire the dispatcher to the world and response callback.
     *
     * @param world game world details
     * @param onResponse callback delivering a response message to a client fd
     */
    GuiDispatcher(game::World &world,
        std::function<void(int fd, std::string)> onResponse);

    GuiDispatcher(const GuiDispatcher &) = delete;
    GuiDispatcher &operator=(const GuiDispatcher &) = delete;
    GuiDispatcher(GuiDispatcher &&) = delete;
    GuiDispatcher &operator=(GuiDispatcher &&) = delete;

    /**
     * @brief Parse and execute the GUI command line for @p fd.
     *
     * @param fd client file descriptor
     * @param line raw command line
     */
    void dispatch(int fd, std::string_view line);

    /**
     * @brief Send the complete initial state sync to a promoted GUI client.
     *
     * @param fd client file descriptor
     * @param frequency current server clock frequency
     */
    void sendInitialSync(int fd, int frequency);

private:
    using Handler =
        std::vector<std::string> (GuiDispatcher::*)(const std::vector<int> &);

    game::World &_world;
    std::function<void(int, std::string)> _onResponse;

    void sendError(int fd, protocol::gui::GuiParseError err);

    /**
     * @brief Build the "msz <width> <height>" line for the current world.
     *
     * @return std::string the formatted map-size line
     */
    [[nodiscard]] std::string buildMszLine() const;

    /**
     * @brief Build the "bct <x> <y> <q0>..<q6>" line for tile (x, y).
     *
     * @param x tile horizontal coordinate
     * @param y tile vertical coordinate
     * @return std::string the formatted tile-content line
     */
    [[nodiscard]] std::string buildBctLine(int x, int y) const;

    void sendSyncMapSize(int fd);
    void sendSyncTiles(int fd);
    void sendSyncTile(int fd, int x, int y);
    void sendSyncTeams(int fd);
    void sendSyncPlayers(int fd);
    void sendSyncPlayer(int fd, const game::Player &player);
    void sendSyncPlayerInventory(int fd, const game::Player &player);
    void sendSyncEggs(int fd);
    void sendSyncTime(int fd, int frequency);

    [[nodiscard]] std::vector<std::string> handleMsz(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handleBct(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handleMct(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handleTna(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handlePpo(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handlePlv(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handlePin(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handleSgt(
        const std::vector<int> &args);
    [[nodiscard]] std::vector<std::string> handleSst(
        const std::vector<int> &args);
};

} // namespace zappy::server::gui

#endif /* !SERVER_GUI_GUIDISPATCHER_HPP_ */
