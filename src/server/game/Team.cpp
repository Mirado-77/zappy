/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Team implementation
*/

#include <utility>

#include "server/game/Team.hpp"
#include "server/game/WorldError.hpp"

/**
 * @brief Build a team with @p slotsTotal free slots.
 *
 * @param name team name
 * @param slotsTotal initial slot capacity
 */
zappy::server::game::Team::Team(std::string name, int slotsTotal)
    : _name(std::move(name)), _slotsTotal(slotsTotal), _slotsUsed(0)
{}

/**
 * @brief Team name.
 *
 * @return const std::string& the name
 */
const std::string &zappy::server::game::Team::name() const noexcept
{
    return _name;
}

/**
 * @brief Total slot capacity.
 *
 * @return int the total slots
 */
int zappy::server::game::Team::slotsTotal() const noexcept
{
    return _slotsTotal;
}

/**
 * @brief Currently reserved slots.
 *
 * @return int the used slots
 */
int zappy::server::game::Team::slotsUsed() const noexcept
{
    return _slotsUsed;
}

/**
 * @brief Remaining free slots.
 *
 * @return int total minus used
 */
int zappy::server::game::Team::slotsFree() const noexcept
{
    return _slotsTotal - _slotsUsed;
}

/**
 * @brief Whether at least one slot is free.
 *
 * @return bool true if a slot is available
 */
bool zappy::server::game::Team::hasFreeSlot() const noexcept
{
    return _slotsUsed < _slotsTotal;
}

/**
 * @brief Reserve one slot.
 *
 * @throws WorldError if the team is full
 */
void zappy::server::game::Team::reserveSlot()
{
    if (!hasFreeSlot())
        throw WorldError("team '" + _name + "' is full");
    ++_slotsUsed;
}

/**
 * @brief Release one slot; no-op if none are used.
 */
void zappy::server::game::Team::releaseSlot() noexcept
{
    if (_slotsUsed > 0)
        --_slotsUsed;
}

/**
 * @brief Grow the capacity by one slot (used by Fork later).
 */
void zappy::server::game::Team::addSlot() noexcept
{
    ++_slotsTotal;
}
