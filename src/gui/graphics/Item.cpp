/*
** EPITECH PROJECT, 2026
** Item.cpp
** File description:
** Item
*/

#include "gui/Item.hpp"
#include "gui/board_data/TileData.hpp"
#include <string>
#include <algorithm>

static size_t resource_sum(const zappy::gui::board_data::Resources resources)
{
    size_t resource_count = 0;

    for (auto resource : resources) {
        if (resource <= 0)
            continue;
        resource_count += resource;
    }
    return resource_count;
}

ColorBuffer::ColorBuffer()
    : set_size(0), r(0), g(0), b(0), a(0)
{
}

void ColorBuffer::add_color(Color color, size_t weight)
{
    set_size += weight;
    r += color.r * weight;
    g += color.g * weight;
    b += color.b * weight;
    a += color.a * weight;
}

Color ColorBuffer::mean()
{
    if (set_size == 0)
        return DEFAULT_COLOR;
    return CLITERAL(Color){
        static_cast<unsigned char>(r / set_size),
        static_cast<unsigned char>(g / set_size),
        static_cast<unsigned char>(b / set_size),
        static_cast<unsigned char>(a / set_size),
    };
}

static Color get_mineral_color(const zappy::gui::board_data::Resources resources)
{
    ColorBuffer color_buffer;
    Color color_it = {};
    size_t resource_count_it = 0;

    for (size_t resource = 0; resource < zappy::gui::board_data::RESOURCE_COUNT; ++resource) {
        if ((ItemType)resource == FOOD)
            continue;
        color_it = mineral_info.find((ItemType)(resource))->second.color;
        resource_count_it = resources[resource];
        if (resource_count_it <= 0)
            continue;
        color_buffer.add_color(color_it, resource_count_it);
    }
    return color_buffer.mean();
}

static std::string get_item_texture_path(const zappy::gui::board_data::Resources resources)
{
    ItemType majority_item_type = (ItemType)*std::max_element(resources.begin(), resources.end());

    return mineral_info.find(majority_item_type)->second.texture_path;
}

static float get_mineral_scale(const zappy::gui::board_data::Resources resources)
{
    ItemType majority_item_type = (ItemType)*std::max_element(resources.begin(), resources.end());
    size_t resource_count = resource_sum(resources);
    double scale_sum = 0;
    float scale_it = 0;
    size_t resource_count_it = 0;

    if (majority_item_type == FOOD)
        return FOOD_SCALE;
    for (size_t resource = 0; resource < zappy::gui::board_data::RESOURCE_COUNT; ++resource) {
        resource_count_it = resources[resource];
        if ((ItemType)resource == FOOD) {
            resource_count -= resource_count_it;
            continue;
        }
        scale_it = mineral_info.find((ItemType)(resource))->second.scale;
        if (resource_count_it <= 0)
            continue;
        scale_sum += resource_count_it * scale_it;
    }
    if (scale_sum == 0)
        return DEFAULT_MEAN;
    return scale_sum / resource_count;
}

Item::Item(const zappy::gui::board_data::Resources resources, Vector3 pos)
    : _resources(resources), _mod(get_item_texture_path(resources), pos)
{
    _mod.change_color(get_mineral_color(resources));
}

void Item::draw()
{
    Color crystal_color = this->_mod.get_color();
    if (
            (crystal_color.r == DEFAULT_COLOR.r)
        &&  (crystal_color.g == DEFAULT_COLOR.g)
        &&  (crystal_color.b == DEFAULT_COLOR.b)
        &&  (crystal_color.a == DEFAULT_COLOR.a)
    )
        return;
    this->_mod.Draw_model(get_mineral_scale(_resources));
}
