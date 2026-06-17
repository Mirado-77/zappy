/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** AiDispatcher implementation
*/

#include <array>
#include <cstddef>
#include <string>
#include <utility>

#include "server/ai/AiDispatcher.hpp"
#include "server/game/OrientationHelper.hpp"
#include "protocol/ai/CommandInfo.hpp"
#include "server/game/Constants.hpp"
#include "server/game/ResourceNameResolver.hpp"
#include "server/game/BroadcastDirection.hpp"

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
      _onResponse(std::move(onResponse)), _lookBuilder(_world)
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
 * @brief Step one tile forward in the player's facing.
 *
 * @param playerId player to move
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeForward(
    int playerId, const std::string &)
{
    const game::Player &player = _world.player(playerId);
    const game::Position delta =
        game::OrientationHelper::forwardDelta(player.orientation());
    const game::Position newPos =
        (player.position() + delta).normalized(_world.width(),
            _world.height());
    _world.movePlayer(playerId, newPos);
    return "ok";
}

/**
 * @brief Turn the player a clockwise quarter turn.
 *
 * @param playerId player to rotate
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeRight(
    int playerId, const std::string &)
{
    const game::Orientation newOrient = game::OrientationHelper::rotateRight(
        _world.player(playerId).orientation());
    _world.rotatePlayer(playerId, newOrient);
    return "ok";
}

/**
 * @brief Turn the player a counter-clockwise quarter turn.
 *
 * @param playerId player to rotate
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeLeft(
    int playerId, const std::string &)
{
    const game::Orientation newOrient = game::OrientationHelper::rotateLeft(
        _world.player(playerId).orientation());
    _world.rotatePlayer(playerId, newOrient);
    return "ok";
}

/**
 * @brief Render the player's vision cone as the Look response.
 *
 * @param playerId player whose vision is rendered
 * @return std::string the bracketed tile list
 */
std::string zappy::server::ai::AiDispatcher::executeLook(
    int playerId, const std::string &)
{
    return _lookBuilder.buildFor(playerId);
}

/**
 * @brief Format the player's inventory as "[food N, linemate N, ...]".
 *
 * @param playerId ID of the player whose inventory is queried
 * @return Inventory string in the AI protocol format
 */
std::string zappy::server::ai::AiDispatcher::executeInventory(
    int playerId, const std::string &)
{
    const server::game::Player &player = _world.player(playerId);
    std::string out = "[";
    bool first = true;

    for (std::size_t index = 0; index < game::RESOURCE_COUNT; ++index) {
        if (!first)
            out.append(", ");
        first = false;
        out.append(game::RESOURCE_NAMES[index]);
        out.append(" ");
        out.append(std::to_string(player.resource(
            static_cast<server::game::ResourceType>(index))));
    }
    out.append("]");
    return out;
}

/**
 * @brief Broadcast @p arg to every other living player, then notify the GUI.
 *
 * @param playerId the broadcasting player
 * @param arg the broadcast text
 * @return std::string always "ok"
 */
std::string zappy::server::ai::AiDispatcher::executeBroadcast(
    int playerId, const std::string &arg)
{
    broadcastToOthers(playerId, arg);
    return "ok";
}

/**
 * @brief Route a broadcast to every other living player with its K value.
 *
 * @param senderId the broadcasting player
 * @param text the broadcast text
 */
void zappy::server::ai::AiDispatcher::broadcastToOthers(
    int senderId, const std::string &text)
{
    const game::Player &sender = _world.player(senderId);
    const game::Position senderPos = sender.position();
    const int width = _world.width();
    const int height = _world.height();

    _world.forEachPlayer([&](int receiverId, const game::Player &receiver) {
        if (receiverId == senderId
            || receiver.state() == game::PlayerState::DEAD)
            return;
        const int k = game::BroadcastDirection::compute(senderPos,
            receiver.position(), receiver.orientation(), width, height);
        _onResponse(receiverId,
            "message " + std::to_string(k) + ", " + text);
    });
    _world.notifyBroadcast(senderId, text);
}

/**
 * @brief Report the number of free slots in the player's team.
 *
 * @param playerId the querying player
 * @return std::string the free-slot count
 */
std::string zappy::server::ai::AiDispatcher::executeConnectNbr(
    int playerId, const std::string &)
{
    const game::Player &player = _world.player(playerId);

    return std::to_string(_world.team(player.team()).slotsFree());
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
 * @brief Pick up one unit of @p arg from the player's tile.
 *
 * @param playerId player taking the resource
 * @param arg resource name to take
 * @return std::string "ok" on success, "ko" on unknown name or empty tile
 */
std::string zappy::server::ai::AiDispatcher::executeTake(
    int playerId, const std::string &arg)
{
    std::string response = "ko";
    const auto type = game::ResourceNameResolver::resolve(arg);

    if (type && _world.takeResourceFromTile(playerId, *type))
        response = "ok";
    return response;
}

/**
 * @brief Drop one unit of @p arg from the player's inventory onto its tile.
 *
 * @param playerId player dropping the resource
 * @param arg resource name to drop
 * @return std::string "ok" on success, "ko" on unknown name or empty inventory
 */
std::string zappy::server::ai::AiDispatcher::executeSet(
    int playerId, const std::string &arg)
{
    std::string response = "ko";
    const auto type = game::ResourceNameResolver::resolve(arg);

    if (type && _world.dropResourceOnTile(playerId, *type))
        response = "ok";
    return response;
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
