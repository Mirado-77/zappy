/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Connect_nbr command tests for the AI dispatcher
*/

#include <criterion/criterion.h>
#include <string>
#include <vector>

#include "server/ai/AiDispatcher.hpp"
#include "server/game/World.hpp"
#include "server/scheduler/Clock.hpp"
#include "server/scheduler/Scheduler.hpp"

using zappy::server::ai::AiDispatcher;
using zappy::server::game::Orientation;
using zappy::server::game::Position;
using zappy::server::game::World;
using zappy::server::scheduler::Duration;
using zappy::server::scheduler::IClock;
using zappy::server::scheduler::Scheduler;
using zappy::server::scheduler::TimePoint;

namespace {

constexpr Position SPAWN{5, 5};

/**
 * @brief Test clock whose time only moves when the test advances it.
 */
class FakeClock : public IClock {
public:
    void advance(Duration d) noexcept { _now += d; }
    TimePoint now() const noexcept override { return _now; }

private:
    TimePoint _now{};
};

/**
 * @brief Bundles a world, scheduler and dispatcher with a 5-slot team.
 */
struct Fixture {
    FakeClock clock;
    Scheduler sched{clock};
    World world{10, 10, {"team"}, 5};
    std::vector<std::string> responses;
    AiDispatcher disp{world, sched, 100,
        [this](int, std::string r) { responses.push_back(std::move(r)); }};

    int addPlayer()
    {
        return world.addPlayer("team", SPAWN, Orientation::NORTH);
    }
};

} // namespace

/* 1. one connected player leaves four free slots */

Test(ai_dispatcher_connect_nbr, reports_remaining_slots)
{
    Fixture fix;
    const int id = fix.addPlayer();
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert_eq(fix.responses.size(), 1U);
    cr_assert_str_eq(fix.responses.front().c_str(), "4");
}

/* 2. more connections shrink the reported free count */

Test(ai_dispatcher_connect_nbr, slots_decrease_with_players)
{
    Fixture fix;
    const int id = fix.addPlayer();
    fix.addPlayer();
    fix.addPlayer();
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert_str_eq(fix.responses.front().c_str(), "2");
}

/* 3. a full team reports zero free slots */

Test(ai_dispatcher_connect_nbr, full_team_zero)
{
    Fixture fix;
    const int id = fix.addPlayer();
    for (int i = 0; i < 4; ++i)
        fix.addPlayer();
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert_str_eq(fix.responses.front().c_str(), "0");
}

/* 4. Connect_nbr resolves instantly, with no time advance */

Test(ai_dispatcher_connect_nbr, instant_no_advance)
{
    Fixture fix;
    const int id = fix.addPlayer();
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert_eq(fix.responses.size(), 1U);
}

/* 5. consecutive instant commands all resolve in one tick */

Test(ai_dispatcher_connect_nbr, consecutive_commands)
{
    Fixture fix;
    const int id = fix.addPlayer();
    fix.disp.dispatch(id, "Connect_nbr");
    fix.disp.dispatch(id, "Connect_nbr");
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert_eq(fix.responses.size(), 3U);
    cr_assert_str_eq(fix.responses[0].c_str(), "4");
    cr_assert_str_eq(fix.responses[1].c_str(), "4");
    cr_assert_str_eq(fix.responses[2].c_str(), "4");
}

/* 6. Connect_nbr behind a timed command does not block the queue */

Test(ai_dispatcher_connect_nbr, does_not_block_queue)
{
    Fixture fix;
    const int id = fix.addPlayer();
    fix.disp.dispatch(id, "Forward");
    fix.disp.dispatch(id, "Connect_nbr");
    fix.sched.tick();
    cr_assert(fix.responses.empty());
    fix.clock.advance(Duration(70));
    fix.sched.tick();
    cr_assert_eq(fix.responses.size(), 2U);
    cr_assert_str_eq(fix.responses[0].c_str(), "ok");
    cr_assert_str_eq(fix.responses[1].c_str(), "4");
}
