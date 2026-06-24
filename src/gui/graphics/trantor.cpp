/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** trantor
*/

#include "gui/trantor.hpp"

trantor::trantor()
: _mod(TRANTOR_PATH), _state(YES),
    _angle(0), _rotattion_axe({0, 0, 0})
{
    this->_mod.load_anim();
    this->_mod.set_anim_index(YES);
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
    this->_mod.set_anim_index(LOOK_ARROUND);
};

void trantor::eject_player()
{
    this->_state = PUNCH;
    this->_mod.set_anim_index(PUNCH);
};

void trantor::death()
{
    this->_state = DIE;
    this->_mod.set_anim_index(DIE);
};

void trantor::set_pos(Vector3 pos)
{
    this->_mod.set_pos(pos);
};

void trantor::draw()
{
    this->_mod.Update_anim();
    this->_mod.move_model({0, 90, 0}, this->_angle, {0.5, 0.5, 0.5});
};
