/*
** EPITECH PROJECT, 2026
** Item.hpp
** File description:
** Item
*/

#ifndef ITEM_HPP_
    #define ITEM_HPP_
    #include <cstddef>
    #include <memory>
    #include "model.hpp"
    #include "gui/board_data/TileData.hpp"

    #define STONE_PATH "src/gui/assets/crystals.glb"
    #define FOOD_PATH "src/gui/assets/octopus_sandwich.glb"

enum ResourceIndex {
    R_FOOD = 0,
    R_LINEMATE = 1,
    R_DERAUMERE = 2,
    R_SIBUR = 3,
    R_MENDIANE = 4,
    R_PHIRAS = 5,
    R_THYSTAME = 6,
};

constexpr std::size_t MINERAL_COUNT = 6;

#define SATISFYING_FOOD_COUNT 5

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
    zappy::gui::board_data::Resources _resources;
    Vector3 _pos;
};

#endif /* !ITEM_HPP_ */
