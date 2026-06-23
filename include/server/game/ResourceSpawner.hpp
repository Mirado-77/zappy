/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Initial resource seeding into a World
*/

#ifndef SERVER_GAME_RESOURCESPAWNER_HPP_
    #define SERVER_GAME_RESOURCESPAWNER_HPP_

    #include <cstdint>
    #include <random>

    #include "server/game/Constants.hpp"

namespace zappy::server::game {

class World;

/**
 * @brief Seeds the map with the initial resource stock at server boot.
 */
class ResourceSpawner {
public:
    /**
     * @brief Build a spawner with a seeded random engine.
     *
     * @param seed value feeding the random engine
     */
    explicit ResourceSpawner(std::uint64_t seed = std::random_device{}());

    /**
     * @brief Drop the target amount of every resource over the grid.
     *
     * @param world world to seed in place
     */
    void spawnInitial(World &world);

    /**
     * @brief Top up every resource short of its target, never removing surplus.
     *
     * @param world world to refill in place
     */
    void refillMissing(World &world);

    /**
     * @brief Target unit count for a resource on a given map size.
     *
     * @param type resource to size
     * @param width map width in tiles
     * @param height map height in tiles
     * @return int the clamped, rounded target count
     */
    [[nodiscard]] static int targetCountFor(ResourceType type,
        int width, int height) noexcept;

private:
    /**
     * @brief Scatter @p count units of @p type over random tiles.
     *
     * @param world world to seed in place
     * @param type resource being dropped
     * @param count number of units to drop
     */
    void spawnOne(World &world, ResourceType type, int count);

    /**
     * @brief Total units of @p type currently present across all tiles.
     *
     * @param world world to scan
     * @param type resource being counted
     * @return int the summed quantity
     */
    [[nodiscard]] int countResourceOnMap(
        const World &world, ResourceType type) const;

    std::mt19937_64 _rng;
};

} // namespace zappy::server::game

#endif /* !SERVER_GAME_RESOURCESPAWNER_HPP_ */
