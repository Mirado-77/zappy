/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** Egg
*/

#include "gui/Egg.hpp"

Egg::Egg()
{
    if (!_mod->isloaded())
        _mod->load();
}

void Egg::draw()
{
    this->_mod->Draw_model();
}

void Egg::set_pos(Vector3 pos)
{
    this->_mod->set_pos(pos);
}
