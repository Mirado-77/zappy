/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Unit tests for the GUI command dispatcher
*/

#include <criterion/criterion.h>
#include <string>
#include <utility>
#include <vector>

#include "server/gui/GuiDispatcher.hpp"
#include "server/game/World.hpp"

using zappy::server::gui::GuiDispatcher;
using zappy::server::game::World;

namespace {

using Reply = std::pair<int, std::string>;

struct Fixture {
    World world{10, 10, {"team"}, 10};
    std::vector<Reply> replies;
    GuiDispatcher disp{world, [this](int fd, std::string r) {
        replies.push_back({fd, std::move(r)});
    }};
};

} // namespace

Test(gui_dispatcher, unknown_command_returns_suc)
{
    Fixture fx;
    fx.disp.dispatch(42, "Bogus");
    cr_assert_eq(fx.replies.size(), 1U);
    cr_assert_eq(fx.replies[0].first, 42);
    cr_assert_str_eq(fx.replies[0].second.c_str(), "suc");
}

Test(gui_dispatcher, bad_parameter_returns_sbp)
{
    Fixture fx;
    fx.disp.dispatch(42, "bct 1");
    fx.disp.dispatch(42, "ppo abc");
    cr_assert_eq(fx.replies.size(), 2U);
    cr_assert_str_eq(fx.replies[0].second.c_str(), "sbp");
    cr_assert_str_eq(fx.replies[1].second.c_str(), "sbp");
}

Test(gui_dispatcher, all_commands_route_to_handlers)
{
    Fixture fx;
    fx.disp.dispatch(42, "msz");
    fx.disp.dispatch(42, "bct 3 5");
    fx.disp.dispatch(42, "mct");
    fx.disp.dispatch(42, "tna");
    fx.disp.dispatch(42, "ppo 1");
    fx.disp.dispatch(42, "plv 1");
    fx.disp.dispatch(42, "pin 1");
    fx.disp.dispatch(42, "sgt");
    fx.disp.dispatch(42, "sst 100");

    cr_assert_eq(fx.replies.size(), 9U);
    cr_assert_str_eq(fx.replies[0].second.c_str(), "stub-msz");
    cr_assert_str_eq(fx.replies[1].second.c_str(), "stub-bct");
    cr_assert_str_eq(fx.replies[2].second.c_str(), "stub-mct");
    cr_assert_str_eq(fx.replies[3].second.c_str(), "stub-tna");
    cr_assert_str_eq(fx.replies[4].second.c_str(), "stub-ppo");
    cr_assert_str_eq(fx.replies[5].second.c_str(), "stub-plv");
    cr_assert_str_eq(fx.replies[6].second.c_str(), "stub-pin");
    cr_assert_str_eq(fx.replies[7].second.c_str(), "stub-sgt");
    cr_assert_str_eq(fx.replies[8].second.c_str(), "stub-sst");
}

Test(gui_dispatcher, dispatch_is_instant)
{
    Fixture fx;
    cr_assert_eq(fx.replies.size(), 0U);
    fx.disp.dispatch(100, "msz");
    cr_assert_eq(fx.replies.size(), 1U);
    cr_assert_eq(fx.replies[0].first, 100);
    cr_assert_str_eq(fx.replies[0].second.c_str(), "stub-msz");
}
