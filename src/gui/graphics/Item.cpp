/*
** EPITECH PROJECT, 2026
** Item.cpp
** File description:
** Item
*/

#include "gui/Item.hpp"
#include "gui/board_data/TileData.hpp"
#include <algorithm>
#include <array>
#include <cstddef>

namespace {

// Target world size (max dimension) every crystal/food is normalized to,
// regardless of its native model size, so nothing spills over the tile.
constexpr float CRYSTAL_TARGET = 0.9f;
constexpr float FOOD_TARGET = 0.8f;
constexpr float ITEM_LIFT = 0.0f;

// Hexagon ring radius for the minerals (well inside the tile half-width 3.0).
constexpr float R = 1.0f;
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

// Per-model normalized scales, computed once from the loaded bounding boxes.
std::array<float, MINERAL_COUNT> g_crystal_scale{};
float g_food_scale = 0.0f;
bool g_scales_ready = false;

float fit_scale(model &m, float target)
{
    const BoundingBox b = m.get_bbox();
    const float max_dim = std::max({b.max.x - b.min.x,
        b.max.y - b.min.y, b.max.z - b.min.z});

    if (max_dim <= 0.0f)
        return target;
    return target / max_dim;
}

} // namespace

Item::Item(const zappy::gui::board_data::Resources resources, Vector3 pos)
    : _resources(resources), _pos(pos)
{
}

void Item::sync(const zappy::gui::board_data::Resources resources)
{
    if (_mod_food && !_mod_food->isloaded())
        _mod_food->load();
    for (auto &crystal : _mod_crystals)
        if (crystal && !crystal->isloaded())
            crystal->load();
    if (!g_scales_ready) {
        for (std::size_t i = 0; i < MINERAL_COUNT; ++i)
            if (_mod_crystals[i])
                g_crystal_scale[i] = fit_scale(*_mod_crystals[i],
                    CRYSTAL_TARGET);
        if (_mod_food)
            g_food_scale = fit_scale(*_mod_food, FOOD_TARGET);
        g_scales_ready = true;
    }
    _resources = resources;
}

void Item::draw()
{
    if (_resources[R_FOOD] > 0 && _mod_food) {
        _mod_food->set_pos({_pos.x + FOOD_OFFSET.x,
            _pos.y + ITEM_LIFT, _pos.z + FOOD_OFFSET.z});
        _mod_food->Draw_model(g_food_scale);
    }
    for (std::size_t i = 0; i < MINERAL_COUNT; ++i) {
        if (_resources[R_LINEMATE + i] <= 0 || !_mod_crystals[i])
            continue;
        _mod_crystals[i]->set_pos({_pos.x + CRYSTAL_OFFSET[i].x,
            _pos.y + ITEM_LIFT, _pos.z + CRYSTAL_OFFSET[i].z});
        _mod_crystals[i]->Draw_model(g_crystal_scale[i]);
    }
}

void Item::set_pos(Vector3 pos)
{
    this->_pos = pos;
}
