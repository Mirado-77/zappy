/*
** EPITECH PROJECT, 2026
** Item.cpp
** File description:
** Item
*/

#include "gui/Item.hpp"
#include "gui/board_data/TileData.hpp"
#include <array>
#include <cstddef>

namespace {

constexpr float FOOD_SCALE = 0.03f;
constexpr float ITEM_LIFT = 0.0f;

const std::array<Color, MINERAL_COUNT> MINERAL_COLOR = {{
    GRAY, RED, PURPLE, ORANGE, YELLOW, GREEN,
}};

const std::array<float, MINERAL_COUNT> MINERAL_SCALE = {{
    0.23f, 0.26f, 0.29f, 0.20f, 0.30f, 0.36f,
}};

constexpr float R = 0.9f;
constexpr float A = R * 0.5f;
constexpr float B = R * 0.8660254f;

const std::array<Vector3, MINERAL_COUNT> CRYSTAL_OFFSET = {{
    {  R, 0.0f,  0.0f},
    {  A, 0.0f,  B},
    { -A, 0.0f,  B},
    { -R, 0.0f,  0.0f},
    { -A, 0.0f, -B},
    {  A, 0.0f, -B},
}};

// Food in the center of the mineral ring.
constexpr Vector3 FOOD_OFFSET = {0.0f, 0.0f, 0.0f};

} // namespace

Item::Item(const zappy::gui::board_data::Resources resources, Vector3 pos)
    : _resources(resources), _pos(pos)
{
}

void Item::sync(const zappy::gui::board_data::Resources resources)
{
    if (_mod_food && !_mod_food->isloaded())
        _mod_food->load();
    if (_mod_mineral && !_mod_mineral->isloaded())
        _mod_mineral->load();
    _resources = resources;
}

void Item::draw()
{
    if (_resources[R_FOOD] > 0 && _mod_food) {
        _mod_food->set_pos({_pos.x + FOOD_OFFSET.x,
            _pos.y + ITEM_LIFT, _pos.z + FOOD_OFFSET.z});
        _mod_food->Draw_model(FOOD_SCALE);
    }
    if (!_mod_mineral)
        return;
    for (std::size_t i = 0; i < MINERAL_COUNT; ++i) {
        if (_resources[R_LINEMATE + i] <= 0)
            continue;
        _mod_mineral->set_pos({_pos.x + CRYSTAL_OFFSET[i].x,
            _pos.y + ITEM_LIFT, _pos.z + CRYSTAL_OFFSET[i].z});
        _mod_mineral->change_color(MINERAL_COLOR[i]);
        _mod_mineral->Draw_model(MINERAL_SCALE[i]);
    }
}

void Item::set_pos(Vector3 pos)
{
    this->_pos = pos;
}
