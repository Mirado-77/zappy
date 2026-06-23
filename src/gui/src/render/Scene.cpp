/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene implementation
*/

#include "gui/render/Scene.hpp"

namespace zappy::gui::render {

namespace {

constexpr float TILE_UNIT = 6.0f;

Vector3 world_pos(int x, int y)
{
    return {static_cast<float>(x) * TILE_UNIT, 0.0f,
        static_cast<float>(y) * TILE_UNIT};
}

} // namespace

void Scene::sync(const state::GameState &state)
{
    ensure_world(state);
    for (const auto &kv : state.players())
        sync_player(kv.first, kv.second);
}

void Scene::ensure_world(const state::GameState &state)
{
    int w = state.width();
    int h = state.height();
    if (w <= 0 || h <= 0)
        return;
    if (_world && w == _width && h == _height)
        return;
    _world = std::make_unique<::world>(w, h);
    _width = w;
    _height = h;
}

void Scene::sync_player(int id, const state::Player &player)
{
    auto [it, inserted] = _slots.try_emplace(id);
    Slot &slot = it->second;
    if (inserted) {
        slot.robot = std::make_unique<::trantor>();
        slot.orientation = static_cast<int>(player.orient);
    }
    slot.robot->set_pos(world_pos(player.x, player.y));
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

} // namespace zappy::gui::render
