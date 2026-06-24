/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** stone
*/

#ifndef ITEM_HPP_
    #define ITEM_HPP_
    #include <iostream>
    #include <map>
    #include "model.hpp"
    #define STONE_PATH "src/gui/assets/crystals.glb"
    #define FOOD_PATH "src/gui/assets/octopus_sandwich.glb"

enum item_type {
    LINMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PIRAS,
    THYSTAME,
    FOOD,
};

class item{
    public:
        item(item_type type);
        ~item() = default;
        void draw();
        void set_pos(Vector3 pos);
    protected:
    private:
        void set_color();

    const std::map<item_type, Color> _find_color =
        {
            {LINMATE, GRAY},
            {DERAUMERE, RED},
            {SIBUR, PURPLE},
            {MENDIANE, ORANGE},
            {PIRAS, YELLOW},
            {THYSTAME, GREEN},
            {FOOD, WHITE},
        };

    const std::map<item_type, std::string> _find_path =
    {
        {LINMATE, STONE_PATH},
        {DERAUMERE, STONE_PATH},
        {SIBUR, STONE_PATH},
        {MENDIANE, STONE_PATH},
        {PIRAS, STONE_PATH},
        {THYSTAME, STONE_PATH},
        {FOOD, FOOD_PATH}
    };
    item_type _type;
    model _mod;
};

#endif /* !ITEM_HPP_ */
