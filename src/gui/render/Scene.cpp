/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene implementation
*/

#include "gui/render/Scene.hpp"
#include <raylib.h>
#include <utility>

namespace zappy::gui {

constexpr float TILE_UNIT = 6.0f;

Vector3 render::Scene::world_pos(int x, int y)
{
    float wave = this->_world->get_wave_pos(x, y);

    return {static_cast<float>(x) * TILE_UNIT, wave,
        static_cast<float>(y) * TILE_UNIT};
}


void render::Scene::sync(const board_data::BoardData &board)
{
    ensure_world(board);
    for (const auto &player : board.players()) {
        sync_player(player.first, player.second);
    }
    for (std::size_t x = 0; x < static_cast<std::size_t>(board.width()); ++x) {
        for (std::size_t y = 0; y < static_cast<std::size_t>(board.height()); ++y) {
            const board_data::TileData &tile = board.get_tile_value(x, y);
            sync_item(tile, std::make_pair(x, y));
        }
    }
    for (const auto &egg : board.eggs()) {
        sync_egg(egg.first, egg.second);
    }
}

void render::Scene::ensure_world(const board_data::BoardData &board)
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

void render::Scene::sync_player(int id, const board_data::PlayerData &player)
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

    // std::vector<TileData> _tiles;
void render::Scene::sync_item(const board_data::TileData &tile, std::pair<std::size_t, std::size_t> coords)
{
    auto [it, inserted] = _items.try_emplace(coords);
    std::unique_ptr<Item> &item_ptr = it->second;
    if (inserted) {
        item_ptr = std::make_unique<::Item>(tile.res, world_pos(coords.first, coords.second));
    } else {
        item_ptr->sync(tile.res);
    }
    item_ptr->set_pos( world_pos(coords.first, coords.second));
}

void render::Scene::rotate_to(Slot &slot, int target)
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

void render::Scene::sync_egg(int id, const board_data::EggData &egg)
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

void render::Scene::sync_incant(int id, const board_data::GraphicalEvent &event)
{
    if (event.kind != board_data::GraphicalEventKind::INCANTATION_START)
        return;
    auto [it, inserted] = this->_incantation.try_emplace(id);
    Incant &slot = it->second;
    if (inserted) {
        slot.incant = std::make_unique<::Incantation>();
    }
    slot.incant->set_pos(world_pos(event.x, event.y));
}

void render::Scene::draw()
{
    if (_world)
        _world->draw_world();
    for (auto &kv : _slots)
        kv.second.robot->draw();
    for (auto &eg : _eggs)
        eg.second.egg->draw();
    for (auto &kv : _items)
        kv.second->draw();
    for (auto &ic : _incantation)
        ic.second.incant->draw();
}

} // namespace zappy::gui
