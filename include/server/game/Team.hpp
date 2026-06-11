/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** A team and its connection-slot accounting
*/

#ifndef SERVER_GAME_TEAM_HPP_
    #define SERVER_GAME_TEAM_HPP_

    #include <string>

namespace zappy::server::game {

/**
 * @brief One team: a name and its used/total slot counters.
 */
class Team {
public:
    /**
     * @brief Build a team with @p slotsTotal free slots.
     *
     * @param name team name
     * @param slotsTotal initial slot capacity
     */
    Team(std::string name, int slotsTotal);

    /**
     * @brief Team name.
     *
     * @return const std::string& the name
     */
    [[nodiscard]] const std::string &name() const noexcept;

    /**
     * @brief Total slot capacity.
     *
     * @return int the total slots
     */
    [[nodiscard]] int slotsTotal() const noexcept;

    /**
     * @brief Currently reserved slots.
     *
     * @return int the used slots
     */
    [[nodiscard]] int slotsUsed() const noexcept;

    /**
     * @brief Remaining free slots.
     *
     * @return int total minus used
     */
    [[nodiscard]] int slotsFree() const noexcept;

    /**
     * @brief Whether at least one slot is free.
     *
     * @return bool true if a slot is available
     */
    [[nodiscard]] bool hasFreeSlot() const noexcept;

    /**
     * @brief Reserve one slot.
     *
     * @throws WorldError if the team is full
     */
    void reserveSlot();

    /**
     * @brief Release one slot; no-op if none are used.
     */
    void releaseSlot() noexcept;

    /**
     * @brief Grow the capacity by one slot (used by Fork later).
     */
    void addSlot() noexcept;

private:
    std::string _name;
    int _slotsTotal;
    int _slotsUsed;
};

} // namespace zappy::server::game

#endif /* !SERVER_GAME_TEAM_HPP_ */
