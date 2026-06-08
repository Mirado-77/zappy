/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** TeamRegistry implementation
*/

#include "server/game/TeamRegistry.hpp"

/**
 * @brief Construct the registry and initialise every team's used-slot count to zero.
 *
 * @param names validated unique team names
 * @param slotsPerTeam maximum simultaneous AI clients per team
 */
zappy::server::game::TeamRegistry::TeamRegistry(
    const std::vector<std::string> &names, int slotsPerTeam)
    : _names(names), _used(), _slotsPerTeam(slotsPerTeam)
{
    for (const auto &name : _names)
        _used.emplace(name, 0);
}

/**
 * @brief Return true if @p name is a configured team.
 *
 * @param name team name to look up
 * @return bool
 */
bool zappy::server::game::TeamRegistry::contains(
    const std::string &name) const noexcept
{
    return _used.count(name) > 0;
}

/**
 * @brief Return true if @p name is known and has at least one free slot.
 *
 * @param name team name to check
 * @return bool
 */
bool zappy::server::game::TeamRegistry::hasSlot(
    const std::string &name) const noexcept
{
    const auto it = _used.find(name);
    return (it != _used.end()) && (it->second < _slotsPerTeam);
}

/**
 * @brief Return the free-slot count for @p name.
 *
 * @param name team name to query
 * @return int number of available slots
 * @throws TeamRegistryError if @p name is not a configured team
 */
int zappy::server::game::TeamRegistry::freeSlots(
    const std::string &name) const
{
    const auto it = _used.find(name);
    if (it == _used.end())
        throw TeamRegistryError("unknown team: " + name);
    return _slotsPerTeam - it->second;
}

/**
 * @brief Consume one slot for @p name.
 *
 * @param name team to reserve for
 * @throws TeamRegistryError if @p name is unknown or the team is already full
 */
void zappy::server::game::TeamRegistry::reserveSlot(const std::string &name)
{
    const auto it = _used.find(name);
    if (it == _used.end())
        throw TeamRegistryError("unknown team: " + name);
    if (it->second >= _slotsPerTeam)
        throw TeamRegistryError("team full: " + name);
    ++it->second;
}

/**
 * @brief Release one slot for @p name; silently ignores unknown teams.
 *
 * @param name team to release a slot for
 */
void zappy::server::game::TeamRegistry::releaseSlot(
    const std::string &name) noexcept
{
    const auto it = _used.find(name);
    if (it != _used.end() && it->second > 0)
        --it->second;
}

/**
 * @brief Return the stable-ordered list of team names.
 *
 * @return const std::vector<std::string>&
 */
const std::vector<std::string> &
zappy::server::game::TeamRegistry::teamNames() const noexcept
{
    return _names;
}
