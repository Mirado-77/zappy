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
    #include "server/gui/GuiLineBuilder.hpp"
    #include "server/client/ClientRegistry.hpp"
    #include "server/config/FeatureFlags.hpp"
    #include "protocol/gui/GuiCommandParser.hpp"

namespace zappy::server::gui {

/**
 * @brief Bundle of admin-authentication dependencies for the dispatcher.
 *
 * Grouped into one parameter so the constructor stays within the argument
 * limit while still owning the registry, feature flags and admin password.
 */
struct GuiAdminDeps {
    client::ClientRegistry &clients;
    const config::FeatureFlags &featureFlags;
    std::string adminPassword;
};

/**
 * @brief Parses incoming GUI command lines and
 * executes the corresponding stub.
 */
class GuiDispatcher {
public:
    /**
     * @brief Wire the dispatcher to the world, admin deps and callbacks.
     *
     * @param world game world details
     * @param adminDeps client registry, feature flags and admin password
     * @param onResponse callback delivering a response message to a client fd
     * @param getFrequency callback to query the current frequency
     * @param setFrequency callback to update the time frequency
     */
    GuiDispatcher(game::World &world, GuiAdminDeps adminDeps,
        std::function<void(int fd, std::string)> onResponse,
        std::function<int()> getFrequency,
        std::function<void(int)> setFrequency);

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
    GuiLineBuilder _lineBuilder;
    std::function<int()> _getFrequency;
    std::function<void(int)> _setFrequency;
    client::ClientRegistry &_clients;
    const config::FeatureFlags &_featureFlags;
    std::string _adminPassword;

    void sendError(int fd, protocol::gui::GuiParseError err);

    /**
     * @brief Run a standard integer-argument command through its handler.
     *
     * @param cmd the parsed command (never ADMIN)
     * @return std::vector<std::string> the response lines
     */
    [[nodiscard]] std::vector<std::string> dispatchStandard(
        const protocol::gui::ParsedGuiCommand &cmd);

    /**
     * @brief Handle the admin authentication command for @p fd.
     *
     * Returns "suc" when the ADMIN flag is off (hides the command), "sbp" on
     * an empty password, "ok" on a correct password (promoting the client),
     * and "ko" otherwise. Never logs nor echoes the password.
     *
     * @param fd client file descriptor
     * @param cmd the parsed admin command (rawArgument holds the password)
     * @return std::vector<std::string> the single response line
     */
    [[nodiscard]] std::vector<std::string> handleAdmin(
        int fd, const protocol::gui::ParsedGuiCommand &cmd);

    /**
     * @brief Promote the GUI client at @p fd to GUI_ADMIN if eligible.
     *
     * @param fd client file descriptor
     * @return bool true if the client was GUI/GUI_ADMIN and is now admin
     */
    [[nodiscard]] bool promoteClientAdmin(int fd);

    /**
     * @brief Build the "msz <width> <height>" line for the current world.
     *
     * @return std::string the formatted map-size line
     */
    [[nodiscard]] std::string buildMszLine() const;


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
