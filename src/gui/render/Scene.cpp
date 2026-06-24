/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene implementation
*/

#include "gui/render/Scene.hpp"

#include <array>
#include <cstddef>

namespace zappy::gui::render {

namespace {

constexpr float TILE_UNIT = 6.0f;
constexpr float ROBOT_HALF = 2.5f;
constexpr float ROBOT_LOW = -1.0f;
constexpr float ROBOT_HIGH = 6.0f;

constexpr std::array<int, board_data::BIOME_COUNT> BIOME_MODELS = {
    0, 1, 2, 3, 4, 4
};

Vector3 world_pos(int x, int y)
{
    return {static_cast<float>(x) * TILE_UNIT, 0.0f,
        static_cast<float>(y) * TILE_UNIT};
}

BoundingBox player_box(int x, int y)
{
    const Vector3 p = world_pos(x, y);
    return {{p.x - ROBOT_HALF, ROBOT_LOW, p.z - ROBOT_HALF},
        {p.x + ROBOT_HALF, ROBOT_HIGH, p.z + ROBOT_HALF}};
}

} // namespace

void Scene::sync(const board_data::BoardData &board)
{
    ensure_world(board);
    apply_biome_models(board);
    for (const auto &kv : board.players())
        sync_player(kv.first, kv.second);
}

void Scene::apply_biome_models(const board_data::BoardData &board)
{
    if (!_world)
        return;
    for (const auto &tile : board.tiles()) {
        const int index = _show_biomes
            ? BIOME_MODELS[static_cast<std::size_t>(tile.biome)] : 0;
        _world->set_tile_model(tile.x, tile.y, index);
    }
}

void Scene::ensure_world(const board_data::BoardData &board)
{
    int w = board.width();
    int h = board.height();
    if (w <= 0 || h <= 0)
        return;
    if (_world && w == _width && h == _height)
        return;
    _world = std::make_unique<::world>(w, h);
    _width = w;
    _height = h;
}

void Scene::sync_player(int id, const board_data::PlayerData &player)
{
    auto [it, inserted] = _slots.try_emplace(id);
    Slot &slot = it->second;
    if (inserted) {
        slot.robot = std::make_unique<::trantor>();
        slot.orientation = static_cast<int>(player.orient);
    }
    slot.robot->set_pos(world_pos(player.x, player.y));
    slot.x = player.x;
    slot.y = player.y;
    rotate_to(slot, static_cast<int>(player.orient));
    if (!player.alive && !slot.dead) {
        slot.robot->death();
        slot.dead = true;
    }
}

void Scene::rotate_to(Slot &slot, int target)
{
    int diff = ((target - slot.orientation) % 4 + 4) % 4;
    if (diff == 0)
        return;
    if (diff == 3) {
        slot.robot->Turn_left();
    } else {
        for (int i = 0; i < diff; ++i)
            slot.robot->Turn_right();
    }
    slot.orientation = target;
}

void Scene::draw()
{
    if (_world)
        _world->draw_world();
    for (auto &kv : _slots)
        kv.second.robot->draw();
}

std::optional<int> Scene::pick_player(Vector2 screen, const Camera &cam) const
{
    const Ray ray = GetScreenToWorldRay(screen, cam);
    float best = 0.0f;
    int found = -1;

    for (const auto &kv : _slots) {
        if (kv.second.dead)
            continue;
        const RayCollision hit =
            GetRayCollisionBox(ray, player_box(kv.second.x, kv.second.y));
        if (hit.hit && (found < 0 || hit.distance < best)) {
            best = hit.distance;
            found = kv.first;
        }
    }
    if (found < 0)
        return std::nullopt;
    return found;
}

} // namespace zappy::gui::render
