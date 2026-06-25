/*
** EPITECH PROJECT, 2026
** assets.cpp
** File description:
** assets
*/

#include "gui/trantor.hpp"
#include "gui/Item.hpp"
#include "gui/Egg.hpp"
#include "gui/LevelUp.hpp"
#include "gui/assets.hpp"

std::unique_ptr<model> trantor::_mod = std::make_unique<model>(TRANTOR_PATH, false);
std::unique_ptr<model> Item::_mod_food = std::make_unique<model>(FOOD_PATH, false);
std::unique_ptr<model> Item::_mod_mineral = std::make_unique<model>(STONE_PATH, false);
std::unique_ptr<model> Egg::_mod = std::make_unique<model>(EGG_PATH, false);
std::unique_ptr<model> LevelUp::_mod = std::make_unique<model>(LEVEL_UP_PATH, false);

void release_shared_models()
{
    trantor::_mod.reset();
    Item::_mod_food.reset();
    Item::_mod_mineral.reset();
    Egg::_mod.reset();
    LevelUp::_mod.reset();
}
