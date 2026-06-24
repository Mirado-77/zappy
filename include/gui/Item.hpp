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

using ItemType = enum item_type {
    LINMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PIRAS,
    THYSTAME,
    FOOD,
};

using ItemInfo = struct ItemInfo_s {
    const Color color;
    const std::string texture_path;
    const float scale;
};

static const std::map<const ItemType, const ItemInfo> mineral_info = {
    {LINMATE, (const ItemInfo){GRAY, STONE_PATH, 0.3}},
    {DERAUMERE, (const ItemInfo){RED, STONE_PATH, 0.3}},
    {SIBUR, (const ItemInfo){PURPLE, STONE_PATH, 0.3}},
    {MENDIANE, (const ItemInfo){ORANGE, STONE_PATH, 0.3}},
    {PIRAS, (const ItemInfo){YELLOW, STONE_PATH, 0.3}},
    {THYSTAME, (const ItemInfo){GREEN, STONE_PATH, 0.3}},
    {FOOD, (const ItemInfo){WHITE, FOOD_PATH, 0.05}},
};

class Item{
public:
    Item(ItemType type, Vector3 pos);
    ~Item() = default;
    void draw();
private:
    ItemType item_type;
    model _mod;
};

#endif /* !ITEM_HPP_ */
