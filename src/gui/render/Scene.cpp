/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene implementation
*/

#include "gui/render/Scene.hpp"

namespace zappy::gui::render {

constexpr float TILE_UNIT = 6.0f;

Vector3 Scene::world_pos(int x, int y)
{
    float wave = this->_world->get_wave_pos(x, y);

    return {static_cast<float>(x) * TILE_UNIT, wave,
        static_cast<float>(y) * TILE_UNIT};
}

void Scene::sync(const board_data::BoardData &board)
{
    ensure_world(board);
    for (const auto &kv : board.players())
        sync_player(kv.first, kv.second);
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

void Scene::sync_egg(int id, const board_data::EggData &egg)
{
    auto [it, inserted] = this->_eggs.try_emplace(id);
    EggSlot &slot = it->second;
    if (inserted) {
        slot.egg = std::make_unique<::Egg>();
    }
    slot.egg->set_pos(world_pos(egg.x, egg.y));
    if (!egg.alive && !slot.dead) {
        slot.dead = true;
    }
}

void Scene::draw()
{
    if (_world)
        _world->draw_world();
    for (auto &kv : _slots)
        kv.second.robot->draw();
    for (auto &eg : _eggs)
        eg.second.egg->draw();
}

} // namespace zappy::gui::render
