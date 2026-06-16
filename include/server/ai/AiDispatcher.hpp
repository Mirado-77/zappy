/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Per-player AI command queue, scheduling and dispatch
*/

#ifndef SERVER_AI_AIDISPATCHER_HPP_
    #define SERVER_AI_AIDISPATCHER_HPP_

    #include <deque>
    #include <functional>
    #include <string>
    #include <string_view>
    #include <unordered_map>

    #include "server/game/World.hpp"
    #include "server/scheduler/Scheduler.hpp"
    #include "protocol/ai/CommandParser.hpp"

namespace zappy::server::ai {

/**
 * @brief Parses, queues, time-schedules and executes AI commands per player.
 */
class AiDispatcher {
public:
    using OnResponse = std::function<void(int, std::string)>;

    /**
     * @brief Wire the dispatcher to the world, scheduler and response sink.
     *
     * @param world game world the commands act upon
     * @param scheduler event queue timing the commands
     * @param frequency reciprocal of the time unit (the f config value)
     * @param onResponse callback delivering a response line to a player
     */
    AiDispatcher(game::World &world, scheduler::Scheduler &scheduler,
        int frequency, OnResponse onResponse);

    AiDispatcher(const AiDispatcher &) = delete;
    AiDispatcher &operator=(const AiDispatcher &) = delete;
    AiDispatcher(AiDispatcher &&) = delete;
    AiDispatcher &operator=(AiDispatcher &&) = delete;

    /**
     * @brief Parse and enqueue @p line for @p playerId.
     *
     * Malformed or unknown lines yield an immediate "ko". A full queue drops
     * the command silently.
     *
     * @param playerId player issuing the command
     * @param line raw command line
     */
    void dispatch(int playerId, std::string_view line);

    /**
     * @brief Cancel the in-flight command and drop the queue for @p playerId.
     *
     * @param playerId player to stop; no-op if unknown
     */
    void stopPlayer(int playerId) noexcept;

private:
    using Handler = std::string (AiDispatcher::*)(int, const std::string &);

    /**
     * @brief Per-player command queue plus its in-flight event id.
     */
    struct PlayerState {
        std::deque<protocol::ai::ParsedCommand> queue;
        scheduler::EventId currentEventId = 0;
    };

    static constexpr std::size_t MAX_PENDING_COMMANDS = 10;

    /**
     * @brief Start as many queued commands as resolve instantly, schedule one.
     *
     * @param playerId player whose queue is advanced
     */
    void startNextCommand(int playerId);

    /**
     * @brief Finish a scheduled command then advance the queue.
     *
     * @param playerId player whose command completed
     * @param cmd the command that just elapsed
     */
    void onCommandComplete(int playerId, protocol::ai::ParsedCommand cmd);

    /**
     * @brief Execute @p cmd and deliver its response to @p playerId.
     *
     * @param playerId player to answer
     * @param cmd command to execute
     */
    void respond(int playerId, const protocol::ai::ParsedCommand &cmd);

    /**
     * @brief Run the handler bound to @p cmd and return its response line.
     *
     * @param playerId player issuing the command
     * @param cmd command to execute
     * @return std::string the response line (no trailing newline)
     */
    [[nodiscard]] std::string executeCommand(int playerId,
        const protocol::ai::ParsedCommand &cmd);

    std::string executeForward(int playerId, const std::string &arg);
    std::string executeRight(int playerId, const std::string &arg);
    std::string executeLeft(int playerId, const std::string &arg);
    std::string executeLook(int playerId, const std::string &arg);
    std::string executeInventory(int playerId, const std::string &arg);
    std::string executeBroadcast(int playerId, const std::string &arg);
    std::string executeConnectNbr(int playerId, const std::string &arg);
    std::string executeFork(int playerId, const std::string &arg);
    std::string executeEject(int playerId, const std::string &arg);
    std::string executeTake(int playerId, const std::string &arg);
    std::string executeSet(int playerId, const std::string &arg);
    std::string executeIncantation(int playerId, const std::string &arg);

    game::World &_world;
    scheduler::Scheduler &_scheduler;
    int _frequency;
    OnResponse _onResponse;
    std::unordered_map<int, PlayerState> _states;
};

} // namespace zappy::server::ai

#endif /* !SERVER_AI_AIDISPATCHER_HPP_ */
