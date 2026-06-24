/*
** EPITECH PROJECT, 2026
** assets.cpp
** File description:
** assets
*/

#include "gui/trantor.hpp"
#include "gui/Item.hpp"
#include "gui/Egg.hpp"

std::unique_ptr<model> trantor::_mod = std::make_unique<model>(TRANTOR_PATH, false);
std::unique_ptr<model> Item::_mod_food = std::make_unique<model>(FOOD_PATH, false);
std::unique_ptr<model> Item::_mod_mineral = std::make_unique<model>(STONE_PATH, false);
std::unique_ptr<model> Egg::_mod = std::make_unique<model>(EGG_PATH, false);
