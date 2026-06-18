/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GuiDispatcher implementation
*/

#include <utility>

#include "server/gui/GuiDispatcher.hpp"

/**
 * @brief Construct a new Gui Dispatcher object.
 *
 * @param world the game world reference
 * @param onResponse the callback to dispatch responses
 */
zappy::server::gui::GuiDispatcher::GuiDispatcher(
    game::World &world,
    std::function<void(int fd, std::string)> onResponse)
    : _world(world), _onResponse(std::move(onResponse))
{
}

/**
 * @brief Dispatch a raw command line from a specific client.
 *
 * @param fd client file descriptor
 * @param line raw command line
 */
void zappy::server::gui::GuiDispatcher::dispatch(
    int fd, std::string_view line)
{
    static constexpr std::array<Handler,
        protocol::gui::GUI_COMMAND_KIND_COUNT> handlers = {
            &GuiDispatcher::handleMsz, &GuiDispatcher::handleBct,
            &GuiDispatcher::handleMct, &GuiDispatcher::handleTna,
            &GuiDispatcher::handlePpo, &GuiDispatcher::handlePlv,
            &GuiDispatcher::handlePin, &GuiDispatcher::handleSgt,
            &GuiDispatcher::handleSst};
    const auto res = protocol::gui::GuiCommandParser::parse(line);

    if (std::holds_alternative<protocol::gui::GuiParseError>(res)) {
        sendError(fd, std::get<protocol::gui::GuiParseError>(res));
    } else {
        const auto cmd = std::get<protocol::gui::ParsedGuiCommand>(res);
        const std::size_t idx = static_cast<std::size_t>(cmd.kind);
        const std::string response = (this->*handlers[idx])(cmd.arguments);
        if (!response.empty())
            _onResponse(fd, response);
    }
}

/**
 * @brief Send appropriate error message to the client on parse error.
 *
 * @param fd client file descriptor
 * @param err type of parsing error encountered
 */
void zappy::server::gui::GuiDispatcher::sendError(
    int fd, protocol::gui::GuiParseError err)
{
    const bool isUnknown =
        (err == protocol::gui::GuiParseError::UNKNOWN_COMMAND);
    _onResponse(fd, isUnknown ? "suc" : "sbp");
}

/**
 * @brief Handle msz command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleMsz(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-msz";
}

/**
 * @brief Handle bct command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleBct(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-bct";
}

/**
 * @brief Handle mct command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleMct(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-mct";
}

/**
 * @brief Handle tna command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleTna(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-tna";
}

/**
 * @brief Handle ppo command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePpo(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-ppo";
}

/**
 * @brief Handle plv command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePlv(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-plv";
}

/**
 * @brief Handle pin command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePin(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-pin";
}

/**
 * @brief Handle sgt command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleSgt(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-sgt";
}

/**
 * @brief Handle sst command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleSst(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-sst";
}

/**
 * @brief Send the complete initial state sync to a promoted GUI client.
 *
 * @param fd client file descriptor
 * @param frequency current server clock frequency
 */
void zappy::server::gui::GuiDispatcher::sendInitialSync(
    int fd, int frequency)
{
    sendSyncMapSize(fd);
    sendSyncTiles(fd);
    sendSyncTeams(fd);
    sendSyncPlayers(fd);
    sendSyncEggs(fd);
    sendSyncTime(fd, frequency);
}

/**
 * @brief Send the map size to the client.
 *
 * @param fd client file descriptor
 */
void zappy::server::gui::GuiDispatcher::sendSyncMapSize(int fd)
{
    const std::string msg = "msz " + std::to_string(_world.width()) + " "
        + std::to_string(_world.height());
    _onResponse(fd, msg);
}

/**
 * @brief Send all tiles to the client.
 *
 * @param fd client file descriptor
 */
void zappy::server::gui::GuiDispatcher::sendSyncTiles(int fd)
{
    for (int y = 0; y < _world.height(); ++y) {
        for (int x = 0; x < _world.width(); ++x) {
            sendSyncTile(fd, x, y);
        }
    }
}

/**
 * @brief Send content of a single tile.
 *
 * @param fd client file descriptor
 * @param x tile horizontal coordinate
 * @param y tile vertical coordinate
 */
void zappy::server::gui::GuiDispatcher::sendSyncTile(
    int fd, int x, int y)
{
    const game::Position pos(x, y);
    const game::Tile &tile = _world.tileAt(pos);
    const std::string msg = "bct " + std::to_string(x) + " "
        + std::to_string(y) + " "
        + std::to_string(tile.resource(game::ResourceType::FOOD)) + " "
        + std::to_string(tile.resource(game::ResourceType::LINEMATE)) + " "
        + std::to_string(tile.resource(game::ResourceType::DERAUMERE)) + " "
        + std::to_string(tile.resource(game::ResourceType::SIBUR)) + " "
        + std::to_string(tile.resource(game::ResourceType::MENDIANE)) + " "
        + std::to_string(tile.resource(game::ResourceType::PHIRAS)) + " "
        + std::to_string(tile.resource(game::ResourceType::THYSTAME));
    _onResponse(fd, msg);
}

/**
 * @brief Send all team names to the client.
 *
 * @param fd client file descriptor
 */
void zappy::server::gui::GuiDispatcher::sendSyncTeams(int fd)
{
    for (const auto &team : _world.teams()) {
        _onResponse(fd, "tna " + team.name());
    }
}

/**
 * @brief Send all player details to the client.
 *
 * @param fd client file descriptor
 */
void zappy::server::gui::GuiDispatcher::sendSyncPlayers(int fd)
{
    _world.forEachPlayer([this, fd](int id, const game::Player &player) {
        (void)id;
        sendSyncPlayer(fd, player);
    });
}

/**
 * @brief Send details of a single player.
 *
 * @param fd client file descriptor
 * @param player player to sync
 */
void zappy::server::gui::GuiDispatcher::sendSyncPlayer(
    int fd, const game::Player &player)
{
    const int pid = player.id();
    const int px = player.position().x();
    const int py = player.position().y();
    const int orient = static_cast<int>(player.orientation());
    const int lvl = player.level();
    const std::string pnw = "pnw " + std::to_string(pid) + " "
        + std::to_string(px) + " " + std::to_string(py) + " "
        + std::to_string(orient) + " " + std::to_string(lvl) + " "
        + player.team();
    _onResponse(fd, pnw);
    _onResponse(fd, "plv " + std::to_string(pid) + " " + std::to_string(lvl));
    sendSyncPlayerInventory(fd, player);
}

/**
 * @brief Send a player's inventory content.
 *
 * @param fd client file descriptor
 * @param player player to sync inventory for
 */
void zappy::server::gui::GuiDispatcher::sendSyncPlayerInventory(
    int fd, const game::Player &player)
{
    const int pid = player.id();
    const int px = player.position().x();
    const int py = player.position().y();
    const std::string msg = "pin " + std::to_string(pid) + " "
        + std::to_string(px) + " " + std::to_string(py) + " "
        + std::to_string(player.resource(game::ResourceType::FOOD)) + " "
        + std::to_string(player.resource(game::ResourceType::LINEMATE)) + " "
        + std::to_string(player.resource(game::ResourceType::DERAUMERE)) + " "
        + std::to_string(player.resource(game::ResourceType::SIBUR)) + " "
        + std::to_string(player.resource(game::ResourceType::MENDIANE)) + " "
        + std::to_string(player.resource(game::ResourceType::PHIRAS)) + " "
        + std::to_string(player.resource(game::ResourceType::THYSTAME));
    _onResponse(fd, msg);
}

/**
 * @brief Send all eggs to the client.
 *
 * @param fd client file descriptor
 */
void zappy::server::gui::GuiDispatcher::sendSyncEggs(int fd)
{
    _world.forEachEgg([this, fd](int id, const game::Egg &egg) {
        const std::string msg = "enw " + std::to_string(id) + " -1 "
            + std::to_string(egg.position().x()) + " "
            + std::to_string(egg.position().y());
        _onResponse(fd, msg);
    });
}

/**
 * @brief Send time frequency to the client.
 *
 * @param fd client file descriptor
 * @param frequency clock frequency of the server
 */
void zappy::server::gui::GuiDispatcher::sendSyncTime(
    int fd, int frequency)
{
    _onResponse(fd, "sgt " + std::to_string(frequency));
}
