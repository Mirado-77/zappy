/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Team slot tracking for the handshake phase
*/

#ifndef GAME_TEAMREGISTRY_HPP_
    #define GAME_TEAMREGISTRY_HPP_

    #include <stdexcept>
    #include <string>
    #include <unordered_map>
    #include <vector>

namespace zappy::server::game {

/**
 * @brief Thrown when an unknown team name or a full team is addressed.
 */
class TeamRegistryError : public std::runtime_error {
public:
    /**
     * @brief Build an error with a descriptive reason.
     *
     * @param reason human-readable description
     */
    explicit TeamRegistryError(const std::string &reason)
        : std::runtime_error(reason) {}
};

/**
 * @brief Tracks slot usage for every configured team.
 *
 * Stable iteration order is guaranteed by an internal name list;
 * fast lookup is provided by an unordered_map of used-slot counts.
 */
class TeamRegistry {
public:
    /**
     * @brief Build the registry from the validated team list and slot cap.
     *
     * @param names unique team names (must not be empty)
     * @param slotsPerTeam maximum simultaneous clients per team (> 0)
     */
    explicit TeamRegistry(const std::vector<std::string> &names, int slotsPerTeam);

    /**
     * @brief Return true if @p name is a known team.
     *
     * @param name team name to look up
     * @return bool
     */
    [[nodiscard]] bool contains(const std::string &name) const noexcept;

    /**
     * @brief Return true if @p name is known and has at least one free slot.
     *
     * @param name team name to check
     * @return bool
     */
    [[nodiscard]] bool hasSlot(const std::string &name) const noexcept;

    /**
     * @brief Return the number of free slots for @p name.
     *
     * @param name team name to query
     * @return int free slot count
     * @throws TeamRegistryError if @p name is unknown
     */
    [[nodiscard]] int freeSlots(const std::string &name) const;

    /**
     * @brief Consume one slot for @p name.
     *
     * @param name team name to reserve a slot for
     * @throws TeamRegistryError if @p name is unknown or the team is full
     */
    void reserveSlot(const std::string &name);

    /**
     * @brief Release one slot for @p name; no-op if @p name is unknown.
     *
     * @param name team name to release a slot for
     */
    void releaseSlot(const std::string &name) noexcept;

    /**
     * @brief Stable-ordered list of all team names.
     *
     * @return const std::vector<std::string>&
     */
    [[nodiscard]] const std::vector<std::string> &teamNames() const noexcept;

private:
    std::vector<std::string> _names;
    std::unordered_map<std::string, int> _used;
    int _slotsPerTeam;
};

} // namespace zappy::server::game

#endif /* !GAME_TEAMREGISTRY_HPP_ */
