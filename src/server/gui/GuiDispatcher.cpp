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
        const auto responses = (this->*handlers[idx])(cmd.arguments);
        for (const auto &line : responses)
            _onResponse(fd, line);
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
 * @brief Build the "msz <width> <height>" line for the current world.
 *
 * @return std::string the formatted map-size line
 */
std::string zappy::server::gui::GuiDispatcher::buildMszLine() const
{
    return "msz " + std::to_string(_world.width()) + " "
        + std::to_string(_world.height());
}

/**
 * @brief Build the "bct <x> <y> <q0>..<q6>" line for tile (x, y).
 *
 * @param x tile horizontal coordinate
 * @param y tile vertical coordinate
 * @return std::string the formatted tile-content line
 */
std::string zappy::server::gui::GuiDispatcher::buildBctLine(int x, int y) const
{
    const game::Tile &tile = _world.tileAt(game::Position(x, y));
    std::string line = "bct " + std::to_string(x) + " " + std::to_string(y);

    for (std::size_t i = 0; i < game::RESOURCE_COUNT; ++i)
        line += " "
            + std::to_string(tile.resource(static_cast<game::ResourceType>(i)));
    return line;
}

/**
 * @brief Handle the msz command: report the map size.
 *
 * @param args command arguments (unused)
 * @return std::vector<std::string> a single "msz <w> <h>" line
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleMsz(
    const std::vector<int> &args)
{
    (void)args;
    return {buildMszLine()};
}

/**
 * @brief Handle the bct command: report a single tile's content.
 *
 * @param args command arguments: x then y
 * @return std::vector<std::string> the bct line, or "sbp" if out of bounds
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleBct(
    const std::vector<int> &args)
{
    const int x = args[0];
    const int y = args[1];
    const bool outOfBounds = (x >= _world.width() || y >= _world.height());

    return outOfBounds ? std::vector<std::string>{"sbp"}
        : std::vector<std::string>{buildBctLine(x, y)};
}

/**
 * @brief Handle the mct command: report every tile's content.
 *
 * @param args command arguments (unused)
 * @return std::vector<std::string> one bct line per tile, Y outer, X inner
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleMct(
    const std::vector<int> &args)
{
    std::vector<std::string> lines;

    (void)args;
    for (int y = 0; y < _world.height(); ++y)
        for (int x = 0; x < _world.width(); ++x)
            lines.push_back(buildBctLine(x, y));
    return lines;
}

/**
 * @brief Handle the tna command: report every team name.
 *
 * @param args command arguments (unused)
 * @return std::vector<std::string> one "tna <name>" line per team
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleTna(
    const std::vector<int> &args)
{
    std::vector<std::string> lines;

    (void)args;
    for (const auto &team : _world.teams())
        lines.push_back("tna " + team.name());
    return lines;
}

/**
 * @brief Build the "ppo #<id> <x> <y> <orientation>" line for a player.
 *
 * @param id player id (assumed to exist)
 * @return std::string the formatted position line
 */
std::string zappy::server::gui::GuiDispatcher::buildPpoLine(int id) const
{
    const game::Player &player = _world.player(id);

    return "ppo #" + std::to_string(id) + " "
        + std::to_string(player.position().x()) + " "
        + std::to_string(player.position().y()) + " "
        + std::to_string(static_cast<int>(player.orientation()));
}

/**
 * @brief Build the "plv #<id> <level>" line for a player.
 *
 * @param id player id (assumed to exist)
 * @return std::string the formatted level line
 */
std::string zappy::server::gui::GuiDispatcher::buildPlvLine(int id) const
{
    return "plv #" + std::to_string(id) + " "
        + std::to_string(_world.player(id).level());
}

/**
 * @brief Build the "pin #<id> <x> <y> <q0>..<q6>" line for a player.
 *
 * @param id player id (assumed to exist)
 * @return std::string the formatted inventory line
 */
std::string zappy::server::gui::GuiDispatcher::buildPinLine(int id) const
{
    const game::Player &player = _world.player(id);
    std::string line = "pin #" + std::to_string(id) + " "
        + std::to_string(player.position().x()) + " "
        + std::to_string(player.position().y());

    for (std::size_t i = 0; i < game::RESOURCE_COUNT; ++i)
        line += " " + std::to_string(
            player.resource(static_cast<game::ResourceType>(i)));
    return line;
}

/**
 * @brief Handle the ppo command: report a player's position and orientation.
 *
 * @param args command arguments: the player id
 * @return std::vector<std::string> the ppo line, or "sbp" if unknown
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handlePpo(
    const std::vector<int> &args)
{
    const int id = args[0];

    return _world.hasPlayer(id) ? std::vector<std::string>{buildPpoLine(id)}
        : std::vector<std::string>{"sbp"};
}

/**
 * @brief Handle the plv command: report a player's level.
 *
 * @param args command arguments: the player id
 * @return std::vector<std::string> the plv line, or "sbp" if unknown
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handlePlv(
    const std::vector<int> &args)
{
    const int id = args[0];

    return _world.hasPlayer(id) ? std::vector<std::string>{buildPlvLine(id)}
        : std::vector<std::string>{"sbp"};
}

/**
 * @brief Handle the pin command: report a player's position and inventory.
 *
 * @param args command arguments: the player id
 * @return std::vector<std::string> the pin line, or "sbp" if unknown
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handlePin(
    const std::vector<int> &args)
{
    const int id = args[0];

    return _world.hasPlayer(id) ? std::vector<std::string>{buildPinLine(id)}
        : std::vector<std::string>{"sbp"};
}

/**
 * @brief Handle sgt command.
 *
 * @param args command arguments
 * @return std::vector<std::string> stub response
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleSgt(
    const std::vector<int> &args)
{
    (void)args;
    return {"stub-sgt"};
}

/**
 * @brief Handle sst command.
 *
 * @param args command arguments
 * @return std::vector<std::string> stub response
 */
std::vector<std::string> zappy::server::gui::GuiDispatcher::handleSst(
    const std::vector<int> &args)
{
    (void)args;
    return {"stub-sst"};
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
    _onResponse(fd, buildMszLine());
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
    _onResponse(fd, buildBctLine(x, y));
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
