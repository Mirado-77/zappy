/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** LevelUp
*/

#include "gui/LevelUp.hpp"
#include "gui/utils.hpp"

#define LEVEL_UP_SCALE 1.5f
#define LEVEL_UP_SPIN 2.0f

LevelUp::LevelUp()
    : _pos({0, 0, 0})
{
    if (!_mod->isloaded()) {
        _mod->load();
        this->_mod->load_anim();
    }
}

void LevelUp::set_pos(Vector3 pos)
{
    this->_pos = pos;
}

void LevelUp::set_shader(Shader shader)
{
    this->_mod->set_shader(shader);
}

void LevelUp::draw()
{
    this->_angle += LEVEL_UP_SPIN;
    this->_mod->set_pos(this->_pos);
    this->_mod->move_model(UP_DIRECTION, this->_angle,
        {LEVEL_UP_SCALE, LEVEL_UP_SCALE, LEVEL_UP_SCALE});
}
