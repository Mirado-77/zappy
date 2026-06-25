/*
** EPITECH PROJECT, 2026
** Item.hpp
** File description:
** Item
*/

#ifndef ITEM_HPP_
    #define ITEM_HPP_
    #include <iostream>
    #include <map>
    #include "model.hpp"
    #define STONE_PATH "src/gui/assets/crystals.glb"
    #define FOOD_PATH "src/gui/assets/octopus_sandwich.glb"
    #include "gui/board_data/TileData.hpp"

using ItemType = enum item_type {
    LINMATE = 0,
    DERAUMERE = 1,
    SIBUR = 2,
    MENDIANE = 3,
    PIRAS = 4,
    THYSTAME = 5,
    FOOD = 6,
};
#define SATISFYING_FOOD_COUNT 5

using ItemInfo = struct ItemInfo_s {
    const Color color;
    const std::string texture_path;
    const float scale;
};

static const Color default_color = { 0, 255, 0, 255 };
#define DEFAULT_MEAN 0.3
#define FOOD_SCALE 0.05
static const std::map<const ItemType, const ItemInfo> mineral_info = {
    {LINMATE, (const ItemInfo){GRAY, STONE_PATH, 0.3}},
    {DERAUMERE, (const ItemInfo){RED, STONE_PATH, 0.3}},
    {SIBUR, (const ItemInfo){PURPLE, STONE_PATH, 0.3}},
    {MENDIANE, (const ItemInfo){ORANGE, STONE_PATH, 0.3}},
    {PIRAS, (const ItemInfo){YELLOW, STONE_PATH, 0.3}},
    {THYSTAME, (const ItemInfo){GREEN, STONE_PATH, 0.3}},
    {FOOD, (const ItemInfo){WHITE, FOOD_PATH, FOOD_SCALE}},
};

class ColorBuffer {
public:
    ColorBuffer();
    void add_color(Color color, size_t weight);
    Color mean();
    size_t set_size;
    size_t r;
    size_t g;
    size_t b;
    size_t a;
};

class Item {
public:
    Item(const zappy::gui::board_data::Resources resources, Vector3 pos);
    ~Item() = default;
    void set_pos(Vector3 pos);
    void draw();
    void sync(const zappy::gui::board_data::Resources resources);
    static std::unique_ptr<model> _mod_food;
    static std::unique_ptr<model> _mod_mineral;
private:
    std::unique_ptr<model>& get_item_model();
    zappy::gui::board_data::Resources _resources;
    Color _mineral_color;
    Vector3 _pos;
};

#endif /* !ITEM_HPP_ */
