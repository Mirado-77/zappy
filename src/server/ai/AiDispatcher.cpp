/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** AiDispatcher implementation
*/

#include <array>
#include <cstddef>
#include <utility>

#include "server/ai/AiDispatcher.hpp"
#include "protocol/ai/CommandInfo.hpp"

/**
 * @brief Bind the dispatcher to the world, scheduler and response sink.
 *
 * @param world game world the commands act upon
 * @param scheduler event queue timing the commands
 * @param frequency reciprocal of the time unit (the f config value)
 * @param onResponse callback delivering a response line to a player
 */
zappy::server::ai::AiDispatcher::AiDispatcher(game::World &world,
    scheduler::Scheduler &scheduler, int frequency, OnResponse onResponse)
    : _world(world), _scheduler(scheduler), _frequency(frequency),
      _onResponse(std::move(onResponse))
{}

/**
 * @brief Parse and enqueue @p line, answering "ko" immediately if malformed.
 *
 * @param playerId player issuing the command
 * @param line raw command line
 */
void zappy::server::ai::AiDispatcher::dispatch(int playerId,
    std::string_view line)
{
    const std::optional<protocol::ai::ParsedCommand> parsed =
        protocol::ai::CommandParser::parse(line);
    if (!parsed) {
        _onResponse(playerId, "ko");
        return;
    }
    PlayerState &state = _states[playerId];
    const std::size_t outstanding =
        state.queue.size() + (state.currentEventId != 0 ? 1U : 0U);
    if (outstanding >= MAX_PENDING_COMMANDS)
        return;
    state.queue.push_back(*parsed);
    if (state.currentEventId == 0)
        startNextCommand(playerId);
}

/**
 * @brief Cancel the in-flight command and drop the queue for @p playerId.
 *
 * @param playerId player to stop; no-op if unknown
 */
void zappy::server::ai::AiDispatcher::stopPlayer(int playerId) noexcept
{
    const auto it = _states.find(playerId);
    if (it == _states.end())
        return;
    if (it->second.currentEventId != 0)
        _scheduler.cancel(it->second.currentEventId);
    _states.erase(it);
}

/**
 * @brief Resolve instantaneous commands now, schedule the first timed one.
 *
 * @param playerId player whose queue is advanced
 */
void zappy::server::ai::AiDispatcher::startNextCommand(int playerId)
{
    PlayerState &state = _states[playerId];
    while (state.currentEventId == 0 && !state.queue.empty()) {
        const protocol::ai::ParsedCommand cmd = state.queue.front();
        state.queue.pop_front();
        const scheduler::Duration delay =
            protocol::ai::CommandInfo::duration(cmd.kind, _frequency);
        if (delay.count() > 0)
            state.currentEventId = _scheduler.schedule(delay,
                [this, playerId, cmd] { onCommandComplete(playerId, cmd); });
        else
            respond(playerId, cmd);
    }
}

/**
 * @brief Finish a scheduled command then advance the queue.
 *
 * @param playerId player whose command completed
 * @param cmd the command that just elapsed
 */
void zappy::server::ai::AiDispatcher::onCommandComplete(int playerId,
    protocol::ai::ParsedCommand cmd)
{
    const auto it = _states.find(playerId);
    if (it == _states.end())
        return;
    it->second.currentEventId = 0;
    respond(playerId, cmd);
    startNextCommand(playerId);
}

/**
 * @brief Execute @p cmd and deliver its response to @p playerId.
 *
 * @param playerId player to answer
 * @param cmd command to execute
 */
void zappy::server::ai::AiDispatcher::respond(int playerId,
    const protocol::ai::ParsedCommand &cmd)
{
    _onResponse(playerId, executeCommand(playerId, cmd));
}

/**
 * @brief Dispatch @p cmd through the per-kind handler table.
 *
 * @param playerId player issuing the command
 * @param cmd command to execute
 * @return std::string the response line ("ko" if the kind is out of range)
 */
std::string zappy::server::ai::AiDispatcher::executeCommand(int playerId,
    const protocol::ai::ParsedCommand &cmd)
{
    static constexpr std::array<Handler, protocol::ai::COMMAND_KIND_COUNT>
        handlers = {
            &AiDispatcher::executeForward, &AiDispatcher::executeRight,
            &AiDispatcher::executeLeft, &AiDispatcher::executeLook,
            &AiDispatcher::executeInventory, &AiDispatcher::executeBroadcast,
            &AiDispatcher::executeConnectNbr, &AiDispatcher::executeFork,
            &AiDispatcher::executeEject, &AiDispatcher::executeTake,
            &AiDispatcher::executeSet, &AiDispatcher::executeIncantation};
    const std::size_t index = static_cast<std::size_t>(cmd.kind);
    std::string result = "ko";
    if (index < handlers.size())
        result = (this->*handlers[index])(playerId, cmd.argument);
    return result;
}

/**
 * @brief Forward command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeForward(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Right command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeRight(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Left command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeLeft(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Look command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeLook(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Inventory command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeInventory(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Broadcast command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeBroadcast(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Connect_nbr command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeConnectNbr(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Fork command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeFork(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Eject command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeEject(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Take command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeTake(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Set command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeSet(
    int, const std::string &)
{
    return "ok";
}

/**
 * @brief Incantation command handler (stub; real effect lands in ZAP-26).
 *
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeIncantation(
    int, const std::string &)
{
    return "ok";
}
