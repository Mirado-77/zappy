/*
** EPITECH PROJECT, 2026
** Item.cpp
** File description:
** Item
*/

#include "gui/Item.hpp"
#include <string>

static Color get_mineral_color(ItemType type)
{
    return mineral_info.find(type)->second.color;
}

static std::string get_mineral_texture_path(ItemType type)
{
    return mineral_info.find(type)->second.texture_path;
}

static float get_mineral_scale(ItemType type)
{
    return mineral_info.find(type)->second.scale;
}

Item::Item(ItemType type, Vector3 pos)
    : item_type(type), _mod(get_mineral_texture_path(type), pos)
{
    _mod.change_color(get_mineral_color(type));
}

void Item::draw()
{
    this->_mod.Draw_model(get_mineral_scale(item_type));
}
