/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** world
*/

#include "world.hpp"

world::world(int x, int y)
{
    for (float i = 0; i < y; i++) {
        this->_map.push_back(init_model(i, x));
    }
}

world::~world()
{

}

void world::draw_world()
{
     for (std::size_t i = 0; i < this->_map.size(); i++) {
        for (std::size_t j = 0; j < this->_map[i].size(); j++) {
            this->_map[i][j]->Draw_model();
        }
    }
}

std::vector<std::unique_ptr<model>> world::init_model(float y, float x)
{
    std::vector<std::unique_ptr<model>> line;
    for (float i = 0; i < x; i++) {
        Vector3 pos = {i * 6, 0, y * 6};
        auto tile = std::make_unique<model>(TILE_PATH);
        tile->move_pos(pos);
        line.emplace_back(std::move(tile));
    }
    return line;
}
