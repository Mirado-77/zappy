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
#include "gui/HealthBarBatch.hpp"

std::unique_ptr<model> trantor::_mod = std::make_unique<model>(TRANTOR_PATH, false);
std::unique_ptr<model> Item::_mod_food = std::make_unique<model>(FOOD_PATH, false);
std::array<std::unique_ptr<model>, MINERAL_COUNT> Item::_mod_crystals = {
    std::make_unique<model>(CRYSTAL_PATHS[0], false),
    std::make_unique<model>(CRYSTAL_PATHS[1], false),
    std::make_unique<model>(CRYSTAL_PATHS[2], false),
    std::make_unique<model>(CRYSTAL_PATHS[3], false),
    std::make_unique<model>(CRYSTAL_PATHS[4], false),
    std::make_unique<model>(CRYSTAL_PATHS[5], false),
};
std::unique_ptr<model> Egg::_mod = std::make_unique<model>(EGG_PATH, false);
std::unique_ptr<HealthBarBatch> trantor::_health_bar_batch = std::make_unique<HealthBarBatch>(false);
std::unique_ptr<model> LevelUp::_mod = std::make_unique<model>(LEVEL_UP_PATH, false);

void release_shared_models()
{
    trantor::_mod.reset();
    Item::_mod_food.reset();
    for (auto &crystal : Item::_mod_crystals)
        crystal.reset();
    Egg::_mod.reset();
    LevelUp::_mod.reset();
}
