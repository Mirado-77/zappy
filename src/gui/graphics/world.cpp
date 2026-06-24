/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** world
*/

#include "gui/world.hpp"

#include <array>
#include <cmath>

namespace {

constexpr float TILE_UNIT = 6.0f;

constexpr std::array<const char *, 6> PALETTE_PATHS = {
    "src/gui/assets/farm_grass-tile.glb",
    "src/gui/assets/autumn_tile.glb",
    "src/gui/assets/farm_desert-tile.glb",
    "src/gui/assets/farm_tundra-tile.glb",
    "src/gui/assets/farm_snow-tile.glb",
    "src/gui/assets/ocean_tile.glb"
};

} // namespace

world::world(int x, int y)
    : _width(x), _height(y)
{
    load_palette();
    _index.assign(y, std::vector<int>(x, 0));
}

world::~world()
{
}

void world::load_palette()
{
    for (const char *path : PALETTE_PATHS)
        this->_palette.push_back(std::make_unique<model>(path));
}

void world::set_tile_model(int x, int y, int model_index)
{
    if (y < 0 || y >= this->_height || x < 0 || x >= this->_width)
        return;
    if (model_index < 0 || model_index >= static_cast<int>(_palette.size()))
        return;
    this->_index[y][x] = model_index;
}

void world::set_shader(Shader shader)
{
    for (auto &tile : this->_palette)
        tile->set_shader(shader);
}

void world::draw_tile(int x, int y, double time, float scale)
{
    const float phase = static_cast<float>(x + y) * 0.4f;
    const float wave = sinf(static_cast<float>(time * 3.0) + phase) * scale;
    model *tile = this->_palette[this->_index[y][x]].get();

    tile->set_pos({static_cast<float>(x) * TILE_UNIT, wave,
        static_cast<float>(y) * TILE_UNIT});
    tile->Draw_model();
}

void world::draw_world()
{
    const double time = GetTime();
    const float scale = (2.0f + static_cast<float>(sin(time))) * 0.2f;

    for (int y = 0; y < this->_height; ++y)
        for (int x = 0; x < this->_width; ++x)
            draw_tile(x, y, time, scale);
}
