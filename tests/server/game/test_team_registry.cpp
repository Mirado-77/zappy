/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Unit tests for TeamRegistry
*/

#include <criterion/criterion.h>
#include <string>
#include <vector>

#include "server/game/TeamRegistry.hpp"

using zappy::server::game::TeamRegistry;
using zappy::server::game::TeamRegistryError;

/* ------------------------------------------------------------------ helpers */

static TeamRegistry make_registry(int slots = 2)
{
    return TeamRegistry({"alpha", "beta"}, slots);
}

/* --------------------------------------------------------------- TeamRegistry */

Test(team_registry, empty_names_list)
{
    TeamRegistry reg(std::vector<std::string>{}, 3);
    cr_assert_eq(reg.teamNames().size(), 0U);
}

Test(team_registry, contains_known_and_unknown)
{
    auto reg = make_registry();
    cr_assert(reg.contains("alpha"));
    cr_assert(reg.contains("beta"));
    cr_assert_not(reg.contains("gamma"));
}

Test(team_registry, has_slot_initially_true)
{
    auto reg = make_registry(1);
    cr_assert(reg.hasSlot("alpha"));
}

Test(team_registry, has_slot_unknown_team_false)
{
    auto reg = make_registry();
    cr_assert_not(reg.hasSlot("ghost"));
}

Test(team_registry, free_slots_matches_cap_initially)
{
    auto reg = make_registry(3);
    cr_assert_eq(reg.freeSlots("alpha"), 3);
}

Test(team_registry, free_slots_throws_on_unknown_team)
{
    auto reg = make_registry();
    cr_assert_throw((void)reg.freeSlots("ghost"), TeamRegistryError);
}

Test(team_registry, reserve_slot_decrements_free_slots)
{
    auto reg = make_registry(2);
    reg.reserveSlot("alpha");
    cr_assert_eq(reg.freeSlots("alpha"), 1);
}

Test(team_registry, reserve_slot_throws_on_unknown_team)
{
    auto reg = make_registry();
    cr_assert_throw(reg.reserveSlot("ghost"), TeamRegistryError);
}

Test(team_registry, reserve_slot_throws_when_full)
{
    auto reg = make_registry(1);
    reg.reserveSlot("alpha");
    cr_assert_throw(reg.reserveSlot("alpha"), TeamRegistryError);
}

Test(team_registry, has_slot_false_when_full)
{
    auto reg = make_registry(1);
    reg.reserveSlot("alpha");
    cr_assert_not(reg.hasSlot("alpha"));
}

Test(team_registry, release_slot_restores_free_slots)
{
    auto reg = make_registry(1);
    reg.reserveSlot("alpha");
    reg.releaseSlot("alpha");
    cr_assert(reg.hasSlot("alpha"));
    cr_assert_eq(reg.freeSlots("alpha"), 1);
}

Test(team_registry, release_slot_no_op_on_unknown_team)
{
    auto reg = make_registry();
    reg.releaseSlot("ghost");
    cr_assert_eq(reg.freeSlots("alpha"), 2);
}

Test(team_registry, release_slot_does_not_go_below_zero)
{
    auto reg = make_registry(2);
    reg.releaseSlot("alpha");
    cr_assert_eq(reg.freeSlots("alpha"), 2);
}

Test(team_registry, team_names_stable_order)
{
    const std::vector<std::string> names{"zeta", "alpha", "mu"};
    TeamRegistry reg(names, 1);
    cr_assert_eq(reg.teamNames(), names);
}
