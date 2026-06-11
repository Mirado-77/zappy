/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** World implementation
*/

#include <algorithm>
#include <utility>

#include "server/game/World.hpp"
#include "server/game/WorldError.hpp"

/**
 * @brief Build an empty world: grid of tiles plus the configured teams.
 *
 * @param width map width in tiles
 * @param height map height in tiles
 * @param teamNames configured team names
 * @param slotsPerTeam slot capacity per team
 */
zappy::server::game::World::World(int width, int height,
    const std::vector<std::string> &teamNames, int slotsPerTeam)
    : _width(width), _height(height),
      _grid(static_cast<std::size_t>(width * height)), _teams(),
      _players(),
      _eggs(), _nextPlayerId(1), _nextEggId(1), _observers(),
      _activeEffects()
{
    for (const auto &name : teamNames)
        _teams.emplace_back(name, slotsPerTeam);
}

/**
 * @brief Map width in tiles.
 *
 * @return int the width
 */
int zappy::server::game::World::width() const noexcept
{
    return _width;
}

/**
 * @brief Map height in tiles.
 *
 * @return int the height
 */
int zappy::server::game::World::height() const noexcept
{
    return _height;
}

/**
 * @brief Row-major grid index of a normalized position.
 *
 * @param pos normalized position
 * @return std::size_t the index into the grid
 */
std::size_t
zappy::server::game::World::tileIndex(Position pos) const noexcept
{
    return static_cast<std::size_t>(pos.y() * _width + pos.x());
}

/**
 * @brief Tile at an already-normalized position.
 *
 * @param pos normalized position
 * @return Tile& the tile
 */
zappy::server::game::Tile &
zappy::server::game::World::tileAt(Position pos) noexcept
{
    return _grid[tileIndex(pos)];
}

/**
 * @brief Tile at an already-normalized position.
 *
 * @param pos normalized position
 * @return const Tile& the tile
 */
const zappy::server::game::Tile &
zappy::server::game::World::tileAt(Position pos) const noexcept
{
    return _grid[tileIndex(pos)];
}

/**
 * @brief Tile at any position, normalizing first.
 *
 * @param pos possibly out-of-range position
 * @return Tile& the tile
 */
zappy::server::game::Tile &
zappy::server::game::World::tileAtRaw(Position pos) noexcept
{
    return _grid[tileIndex(pos.normalized(_width, _height))];
}

/**
 * @brief Tile at any position, normalizing first.
 *
 * @param pos possibly out-of-range position
 * @return const Tile& the tile
 */
const zappy::server::game::Tile &
zappy::server::game::World::tileAtRaw(Position pos) const noexcept
{
    return _grid[tileIndex(pos.normalized(_width, _height))];
}

/**
 * @brief Whether @p name is a known team.
 *
 * @param name team name
 * @return bool true if known
 */
bool zappy::server::game::World::hasTeam(
    const std::string &name) const noexcept
{
    return std::any_of(_teams.begin(), _teams.end(),
        [&name](const Team &t) { return t.name() == name; });
}

/**
 * @brief Mutable team by name.
 *
 * @param name team name
 * @return Team& the team
 * @throws WorldError if unknown
 */
zappy::server::game::Team &
zappy::server::game::World::team(const std::string &name)
{
    const auto it = std::find_if(_teams.begin(), _teams.end(),
        [&name](const Team &t) { return t.name() == name; });
    if (it == _teams.end())
        throw WorldError("unknown team '" + name + "'");
    return *it;
}

/**
 * @brief Read-only team by name.
 *
 * @param name team name
 * @return const Team& the team
 * @throws WorldError if unknown
 */
const zappy::server::game::Team &
zappy::server::game::World::team(const std::string &name) const
{
    const auto it = std::find_if(_teams.begin(), _teams.end(),
        [&name](const Team &t) { return t.name() == name; });
    if (it == _teams.end())
        throw WorldError("unknown team '" + name + "'");
    return *it;
}

/**
 * @brief All teams in configuration order.
 *
 * @return const std::vector<Team>& the teams
 */
const std::vector<zappy::server::game::Team> &
zappy::server::game::World::teams() const noexcept
{
    return _teams;
}

/**
 * @brief Whether a player with @p id exists.
 *
 * @param id player id
 * @return bool true if present
 */
bool zappy::server::game::World::hasPlayer(int id) const noexcept
{
    return _players.contains(id);
}

/**
 * @brief Mutable player by id.
 *
 * @param id player id
 * @return Player& the player
 * @throws WorldError if unknown
 */
zappy::server::game::Player &zappy::server::game::World::player(int id)
{
    const auto it = _players.find(id);
    if (it == _players.end())
        throw WorldError("unknown player id " + std::to_string(id));
    return it->second;
}

/**
 * @brief Read-only player by id.
 *
 * @param id player id
 * @return const Player& the player
 * @throws WorldError if unknown
 */
const zappy::server::game::Player &
zappy::server::game::World::player(int id) const
{
    const auto it = _players.find(id);
    if (it == _players.end())
        throw WorldError("unknown player id " + std::to_string(id));
    return it->second;
}

/**
 * @brief Create a player, reserve a team slot, place it on its tile.
 *
 * @param team owning team name
 * @param pos starting tile
 * @param orient starting facing
 * @return int the new player id
 * @throws WorldError if the team is unknown or full
 */
int zappy::server::game::World::addPlayer(
    const std::string &team, Position pos, Orientation orient)
{
    Team &owner = this->team(team);
    owner.reserveSlot();
    const int id = _nextPlayerId++;
    _players.emplace(id, Player(id, team, pos, orient));
    tileAt(pos).addPlayer(id);
    notifyPlayerAdded(id);
    notifyTeamSlotsChanged(team);
    return id;
}

/**
 * @brief Remove a player; release its slot if still ALIVE.
 *
 * @param id player id
 * @throws WorldError if unknown
 */
void zappy::server::game::World::removePlayer(int id)
{
    Player &target = player(id);
    const std::string teamName = target.team();
    const bool wasAlive = target.state() == PlayerState::ALIVE;
    tileAt(target.position()).removePlayer(id);
    _players.erase(id);
    notifyPlayerRemoved(id);
    if (wasAlive) {
        team(teamName).releaseSlot();
        notifyTeamSlotsChanged(teamName);
    }
}

/**
 * @brief Whether an egg with @p id exists.
 *
 * @param id egg id
 * @return bool true if present
 */
bool zappy::server::game::World::hasEgg(int id) const noexcept
{
    return _eggs.contains(id);
}

/**
 * @brief Mutable egg by id.
 *
 * @param id egg id
 * @return Egg& the egg
 * @throws WorldError if unknown
 */
zappy::server::game::Egg &zappy::server::game::World::egg(int id)
{
    const auto it = _eggs.find(id);
    if (it == _eggs.end())
        throw WorldError("unknown egg id " + std::to_string(id));
    return it->second;
}

/**
 * @brief Read-only egg by id.
 *
 * @param id egg id
 * @return const Egg& the egg
 * @throws WorldError if unknown
 */
const zappy::server::game::Egg &
zappy::server::game::World::egg(int id) const
{
    const auto it = _eggs.find(id);
    if (it == _eggs.end())
        throw WorldError("unknown egg id " + std::to_string(id));
    return it->second;
}

/**
 * @brief Create an egg and place it on its tile.
 *
 * @param team owning team name
 * @param pos tile to lay the egg on
 * @return int the new egg id
 */
int zappy::server::game::World::addEgg(
    const std::string &team, Position pos)
{
    const int id = _nextEggId++;
    _eggs.emplace(id, Egg(id, team, pos));
    tileAt(pos).addEgg(id);
    notifyEggAdded(id);
    return id;
}

/**
 * @brief Remove an egg from the world and its tile.
 *
 * @param id egg id
 * @throws WorldError if unknown
 */
void zappy::server::game::World::removeEgg(int id)
{
    Egg &target = egg(id);
    tileAt(target.position()).removeEgg(id);
    _eggs.erase(id);
    notifyEggRemoved(id);
}

/**
 * @brief Move a player to @p newPos (assumed normalized).
 *
 * @param playerId player id
 * @param newPos destination tile
 * @throws WorldError if unknown
 */
void zappy::server::game::World::movePlayer(int playerId, Position newPos)
{
    Player &target = player(playerId);
    const Position oldPos = target.position();
    tileAt(oldPos).removePlayer(playerId);
    tileAt(newPos).addPlayer(playerId);
    target.setPosition(newPos);
    notifyPlayerMoved(playerId, oldPos, newPos);
}

/**
 * @brief Rotate a player to @p newOrient.
 *
 * @param playerId player id
 * @param newOrient new facing
 * @throws WorldError if unknown
 */
void zappy::server::game::World::rotatePlayer(
    int playerId, Orientation newOrient)
{
    player(playerId).setOrientation(newOrient);
    notifyPlayerRotated(playerId);
}

/**
 * @brief Set a player's elevation level.
 *
 * @param playerId player id
 * @param newLevel new level
 * @throws WorldError if unknown
 */
void zappy::server::game::World::setPlayerLevel(int playerId, int newLevel)
{
    player(playerId).setLevel(newLevel);
    notifyPlayerLevelChanged(playerId);
}

/**
 * @brief Move one unit of @p type from the player's tile to its inventory.
 *
 * @param playerId player id
 * @param type resource to take
 * @return bool false if the tile has none
 * @throws WorldError if unknown
 */
bool zappy::server::game::World::takeResourceFromTile(
    int playerId, ResourceType type)
{
    Player &target = player(playerId);
    const bool ok = tileAt(target.position()).removeResource(type, 1);
    if (ok) {
        target.addResource(type, 1);
        notifyTileChanged(target.position());
        notifyPlayerInventoryChanged(playerId);
    }
    return ok;
}

/**
 * @brief Move one unit of @p type from the player's inventory to its tile.
 *
 * @param playerId player id
 * @param type resource to drop
 * @return bool false if the player has none
 * @throws WorldError if unknown
 */
bool zappy::server::game::World::dropResourceOnTile(
    int playerId, ResourceType type)
{
    Player &target = player(playerId);
    const bool ok = target.removeResource(type, 1);
    if (ok) {
        tileAt(target.position()).addResource(type, 1);
        notifyTileChanged(target.position());
        notifyPlayerInventoryChanged(playerId);
    }
    return ok;
}

/**
 * @brief Kill a player: mark DEAD, remove from tile, release its slot.
 *
 * @param playerId player id
 * @throws WorldError if unknown
 */
void zappy::server::game::World::killPlayer(int playerId)
{
    Player &target = player(playerId);
    target.setState(PlayerState::DEAD);
    tileAt(target.position()).removePlayer(playerId);
    team(target.team()).releaseSlot();
    notifyPlayerStateChanged(playerId);
    notifyPlayerRemoved(playerId);
}

/**
 * @brief Hatch an egg (state HATCHED); the egg is not removed here.
 *
 * @param eggId egg id
 * @throws WorldError if unknown
 */
void zappy::server::game::World::hatchEgg(int eggId)
{
    egg(eggId).setState(EggState::HATCHED);
    notifyEggHatched(eggId);
}

/**
 * @brief Overwrite a tile's resource quantity.
 *
 * @param pos tile position
 * @param type resource to set
 * @param qty new quantity
 */
void zappy::server::game::World::setTileResource(
    Position pos, ResourceType type, int qty)
{
    tileAt(pos).setResource(type, qty);
    notifyTileChanged(pos);
}

/**
 * @brief Add to a tile's resource quantity.
 *
 * @param pos tile position
 * @param type resource to add
 * @param qty quantity to add
 */
void zappy::server::game::World::addTileResource(
    Position pos, ResourceType type, int qty)
{
    tileAt(pos).addResource(type, qty);
    notifyTileChanged(pos);
}

/**
 * @brief Register an observer for subsequent mutations.
 *
 * @param observer observer to add
 */
void zappy::server::game::World::addObserver(IWorldObserver &observer)
{
    _observers.emplace_back(observer);
}

/**
 * @brief Unregister an observer; no-op if not registered.
 *
 * @param observer observer to remove
 */
void zappy::server::game::World::removeObserver(IWorldObserver &observer)
{
    std::erase_if(_observers,
        [&observer](const std::reference_wrapper<IWorldObserver> &o) {
            return &o.get() == &observer;
        });
}

/**
 * @brief Notify observers a tile changed.
 *
 * @param pos the affected tile
 */
void zappy::server::game::World::notifyTileChanged(Position pos)
{
    for (auto &observer : _observers)
        observer.get().onTileChanged(pos);
}

/**
 * @brief Notify observers a player was added.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerAdded(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerAdded(id);
}

/**
 * @brief Notify observers a player moved.
 *
 * @param id the player id
 * @param oldPos previous tile
 * @param newPos new tile
 */
void zappy::server::game::World::notifyPlayerMoved(
    int id, Position oldPos, Position newPos)
{
    for (auto &observer : _observers)
        observer.get().onPlayerMoved(id, oldPos, newPos);
}

/**
 * @brief Notify observers a player rotated.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerRotated(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerRotated(id);
}

/**
 * @brief Notify observers a player's level changed.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerLevelChanged(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerLevelChanged(id);
}

/**
 * @brief Notify observers a player's inventory changed.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerInventoryChanged(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerInventoryChanged(id);
}

/**
 * @brief Notify observers a player's state changed.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerStateChanged(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerStateChanged(id);
}

/**
 * @brief Notify observers a player was removed.
 *
 * @param id the player id
 */
void zappy::server::game::World::notifyPlayerRemoved(int id)
{
    for (auto &observer : _observers)
        observer.get().onPlayerRemoved(id);
}

/**
 * @brief Notify observers an egg was added.
 *
 * @param id the egg id
 */
void zappy::server::game::World::notifyEggAdded(int id)
{
    for (auto &observer : _observers)
        observer.get().onEggAdded(id);
}

/**
 * @brief Notify observers an egg hatched.
 *
 * @param id the egg id
 */
void zappy::server::game::World::notifyEggHatched(int id)
{
    for (auto &observer : _observers)
        observer.get().onEggHatched(id);
}

/**
 * @brief Notify observers an egg was removed.
 *
 * @param id the egg id
 */
void zappy::server::game::World::notifyEggRemoved(int id)
{
    for (auto &observer : _observers)
        observer.get().onEggRemoved(id);
}

/**
 * @brief Notify observers a team's slot count changed.
 *
 * @param teamName the affected team
 */
void zappy::server::game::World::notifyTeamSlotsChanged(
    const std::string &teamName)
{
    for (auto &observer : _observers)
        observer.get().onTeamSlotsChanged(teamName);
}
