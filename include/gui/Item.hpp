/*
** EPITECH PROJECT, 2026
** Item.hpp
** File description:
** Item
*/

#ifndef ITEM_HPP_
    #define ITEM_HPP_
    #include <array>
    #include <cstddef>
    #include <memory>
    #include "model.hpp"
    #include "gui/board_data/TileData.hpp"

    #define FOOD_PATH "src/gui/assets/octopus_sandwich.glb"

// Resource indices, matching the BoardData/BCT order
// (food first, then the six minerals).
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

// One distinct crystal model per mineral type (index 0 == linemate).
static const std::array<const char *, MINERAL_COUNT> CRYSTAL_PATHS = {
    "src/gui/assets/cristaux/crystal1.glb",
    "src/gui/assets/cristaux/crystal2.glb",
    "src/gui/assets/cristaux/crystal3.glb",
    "src/gui/assets/cristaux/crystal4.glb",
    "src/gui/assets/cristaux/crystal5.glb",
    "src/gui/assets/cristaux/crysta6.glb",
};

class Item {
public:
    Item(const zappy::gui::board_data::Resources resources, Vector3 pos);
    ~Item() = default;
    void set_pos(Vector3 pos);
    void draw();
    void sync(const zappy::gui::board_data::Resources resources);
    static std::unique_ptr<model> _mod_food;
    static std::array<std::unique_ptr<model>, MINERAL_COUNT> _mod_crystals;
private:
    zappy::gui::board_data::Resources _resources;
    Vector3 _pos;
};

#endif /* !ITEM_HPP_ */
