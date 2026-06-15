/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Initial resource seeding into a World
*/

#include "server/game/ResourceSpawner.hpp"

#include <algorithm>
#include <cmath>

#include "server/game/Position.hpp"
#include "server/game/ResourceDensity.hpp"
#include "server/game/World.hpp"

zappy::server::game::ResourceSpawner::ResourceSpawner(std::uint64_t seed)
    : _rng(seed)
{}

void zappy::server::game::ResourceSpawner::spawnInitial(World &world)
{
    const int width = world.width();
    const int height = world.height();
    for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
        const ResourceType type = static_cast<ResourceType>(i);
        spawnOne(world, type, targetCountFor(type, width, height));
    }
}

void zappy::server::game::ResourceSpawner::refillMissing(World &world)
{
    const int width = world.width();
    const int height = world.height();
    for (std::size_t i = 0; i < RESOURCE_COUNT; ++i) {
        const ResourceType type = static_cast<ResourceType>(i);
        const int target = targetCountFor(type, width, height);
        const int missing = target - countResourceOnMap(world, type);
        if (missing > 0)
            spawnOne(world, type, missing);
    }
}

int zappy::server::game::ResourceSpawner::countResourceOnMap(
    const World &world, ResourceType type) const
{
    int total = 0;
    for (int y = 0; y < world.height(); ++y)
        for (int x = 0; x < world.width(); ++x)
            total += world.tileAt(Position(x, y)).resource(type);
    return total;
}

int zappy::server::game::ResourceSpawner::targetCountFor(
    ResourceType type, int width, int height) noexcept
{
    const double area = static_cast<double>(width) * height;
    const double raw = std::round(area * ResourceDensity::densityFor(type));
    return std::max(1, static_cast<int>(raw));
}

void zappy::server::game::ResourceSpawner::spawnOne(
    World &world, ResourceType type, int count)
{
    std::uniform_int_distribution<int> xDist(0, world.width() - 1);
    std::uniform_int_distribution<int> yDist(0, world.height() - 1);
    for (int i = 0; i < count; ++i)
        world.addTileResource(Position(xDist(_rng), yDist(_rng)), type, 1);
}
