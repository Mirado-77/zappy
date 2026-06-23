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
    auto time = GetTime();
    for (std::size_t i = 0; i < this->_map.size(); i++) {
        for (std::size_t j = 0; j < this->_map[i].size(); j++) {
            this->_map[i][j]->Draw_model();
        }
    }
    float scale = (2.0f + (float)sin(time))*0.2f;
    this->model_animation(time, scale);
}

std::vector<std::unique_ptr<model>> world::init_model(float y, float x)
{
    std::vector<std::unique_ptr<model>> line;
    for (float i = 0; i < x; i++) {
        Vector3 pos = {i * 6, 0, y * 6};
        auto tile = std::make_unique<model>(TILE_PATH);
        tile->set_pos(pos);
        tile->set_base_pos(pos);
        line.emplace_back(std::move(tile));
    }
    return line;
}

void world::model_animation(std::double_t time, float scale)
{
    for (std::size_t i = 0; i < this->_map.size(); i++) {
        for (std::size_t j = 0; j < this->_map[i].size(); j++) {
            Vector3 base = this->_map[i][j]->get_base_pos();
            float phase  = (float)(i + j) * 0.4f;
            float wave   = sinf((float)(time * 3.0f) + phase) * scale;
            Vector3 pos  = base;
            pos.y        = base.y + wave;
            this->_map[i][j]->set_pos(pos);
        }
    }
}
