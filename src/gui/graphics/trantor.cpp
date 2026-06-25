/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** trantor
*/

#include "gui/trantor.hpp"
#include "gui/utils.hpp"

trantor::trantor()
    : _pos({0, 0, 0}),
    _state(YES),
    _angle(NO_ANGLE)
{
    if (!_mod->isloaded()) {
        _mod->load();
        this->_mod->load_anim();
    }
};

void trantor::Turn_left()
{
    this->_angle += 90;
};

void trantor::Turn_right()
{
    this->_angle -= 90;
};

void trantor::look_around()
{
    this->_state = LOOK_ARROUND;
};

void trantor::eject_player()
{
    this->_state = PUNCH;
};

void trantor::death()
{
    this->_state = DIE;
};

void trantor::set_pos(Vector3 pos)
{
    _pos = pos;
};

void trantor::set_shader(Shader shader)
{
    this->_mod->set_shader(shader);
};

void trantor::Update_anim()
{
    this->_mod->Update_anim();

};

void trantor::draw()
{
    this->_mod->set_pos(_pos);
    this->_mod->set_anim_index(_state);
    this->_mod->move_model(UP_DIRECTION, this->_angle, DEFAULT_TRANTORIAN_SCALE);
};
