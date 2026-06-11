/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Unit tests for Team
*/

#include <criterion/criterion.h>

#include "server/game/Team.hpp"
#include "server/game/WorldError.hpp"

using zappy::server::game::Team;
using zappy::server::game::WorldError;

/* construction */

Test(team, defaults)
{
    const Team t("alpha", 3);
    cr_assert_str_eq(t.name().c_str(), "alpha");
    cr_assert_eq(t.slotsTotal(), 3);
    cr_assert_eq(t.slotsUsed(), 0);
    cr_assert_eq(t.slotsFree(), 3);
    cr_assert(t.hasFreeSlot());
}

/* slots */

Test(team, reserve_and_release)
{
    Team t("alpha", 2);
    t.reserveSlot();
    cr_assert_eq(t.slotsUsed(), 1);
    cr_assert_eq(t.slotsFree(), 1);
    t.releaseSlot();
    cr_assert_eq(t.slotsUsed(), 0);
}

Test(team, reserve_until_full_then_throws)
{
    Team t("alpha", 1);
    t.reserveSlot();
    cr_assert_not(t.hasFreeSlot());
    cr_assert_throw(t.reserveSlot(), WorldError);
}

Test(team, release_on_zero_is_noop)
{
    Team t("alpha", 2);
    t.releaseSlot();
    cr_assert_eq(t.slotsUsed(), 0);
}

Test(team, add_slot_increments_total)
{
    Team t("alpha", 1);
    t.addSlot();
    cr_assert_eq(t.slotsTotal(), 2);
    cr_assert_eq(t.slotsFree(), 2);
}
