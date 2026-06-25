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
        return default_color;
    return {
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

std::unique_ptr<model>& Item::get_item_model()
{
    ItemType majority_item_type = (ItemType)*std::max_element(_resources.begin(), _resources.end());
    std::string texture_path = mineral_info.find(majority_item_type)->second.texture_path;

    if (texture_path == FOOD_PATH) {
        return this->_mod_food;
    } else
        return this->_mod_mineral;
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
    : _resources(resources), _pos(pos)
{
    _mineral_color = get_mineral_color(resources);
}

void Item::sync(const zappy::gui::board_data::Resources resources)
{
    if (!_mod_food->isloaded())
        _mod_food->load();
    if (!_mod_mineral->isloaded())
        _mod_mineral->load();
    _mineral_color = get_mineral_color(resources);
    _resources = resources;
}

void Item::draw()
{
    if (
            (_mineral_color.r == default_color.r)
        &&  (_mineral_color.g == default_color.g)
        &&  (_mineral_color.b == default_color.b)
        &&  (_mineral_color.a == default_color.a)
    )
        return;
    std::unique_ptr<model>& model_to_display = get_item_model();
    model_to_display->set_pos(_pos);
    model_to_display->change_color(_mineral_color);
    model_to_display->Draw_model(get_mineral_scale(_resources));
}

void Item::set_pos(Vector3 pos)
{
    this->_pos = pos;
}