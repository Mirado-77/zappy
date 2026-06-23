/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** item
*/

#include "item.hpp"
#include <string>

item::item(item_type type)
: _type(type), _mod(_find_path.find(type)->second)
{
}

void item::set_color()
{
    auto it = this->_find_color.find(this->_type);
    this->_mod.change_color(it->second);
}

void item::set_pos(Vector3 pos)
{
    this->_mod.set_pos(pos);
}

void item::draw()
{
    float scale = 0.3;
    if (this->_type == FOOD)
        scale = 0.05;
    this->_mod.Draw_model(scale);
}
