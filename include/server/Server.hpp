/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Top-level server orchestrator
*/

#ifndef SERVER_SERVER_HPP_
    #define SERVER_SERVER_HPP_

    #include "server/cli/ServerConfig.hpp"
    #include "server/client/ClientRegistry.hpp"
    #include "server/game/FoodScheduler.hpp"
    #include "server/game/RefillScheduler.hpp"
    #include "server/game/ResourceSpawner.hpp"
    #include "server/game/World.hpp"
    #include "server/handshake/HandshakeHandler.hpp"
    #include "server/scheduler/Clock.hpp"
    #include "server/scheduler/Scheduler.hpp"
    #include "net/Listener.hpp"
    #include "net/PollLoop.hpp"

namespace zappy::server {

/**
 * @brief Top-level orchestrator owning the listener, poll loop and client registry.
 */
class Server {
public:
    /**
     * @brief Build the server from a validated configuration.
     *
     * @param config the parsed CLI configuration
     */
    explicit Server(const cli::ServerConfig &config);

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    /**
     * @brief Run the event loop until SIGINT or stop().
     */
    void run();

private:
    /**
     * @brief Drain all pending accept()s on the listening socket.
     */
    void acceptPendingClients();

    /**
     * @brief Dispatch POLLIN / POLLOUT events for one client fd.
     *
     * @param clientFd the file descriptor that got events
     * @param revents bitmask returned by poll for this fd
     */
    void handleClientEvents(int clientFd, short revents);

    /**
     * @brief Process a POLLIN event; return true if the client was dropped.
     *
     * @param clientFd the readable file descriptor
     * @return true if the client was dropped
     */
    bool handlePollin(int clientFd);

    /**
     * @brief Process a POLLOUT event; return true if the client was dropped.
     *
     * @param clientFd the writable file descriptor
     * @return true if the client was dropped
     */
    bool handlePollout(int clientFd);

    /**
     * @brief Unregister @p clientFd from the poll loop and remove it from the registry.
     *
     * @param clientFd file descriptor to drop
     */
    void dropClient(int clientFd);

    /**
     * @brief Adjust the poll mask to reflect whether POLLOUT is needed.
     *
     * @param clientFd file descriptor to retune
     */
    void retunePollMask(int clientFd);

    /**
     * @brief Route all ready messages from @p clientFd to the appropriate handler.
     *
     * @param clientFd file descriptor whose buffer has ready messages
     * @return true if the client was dropped during dispatch
     */
    bool dispatchReadyMessages(int clientFd);

    /**
     * @brief Handle a player reaching zero food: notify, kill and drop.
     *
     * @param playerId starving player id reported by the food scheduler
     */
    void onPlayerStarved(int playerId);

    cli::ServerConfig _config;
    net::Listener _listener;
    net::PollLoop _poll;
    game::World _world;
    game::ResourceSpawner _resourceSpawner;
    client::ClientRegistry _clients;
    scheduler::SteadyClock _clock;
    scheduler::Scheduler _scheduler{_clock};
    game::RefillScheduler _refillScheduler;
    game::FoodScheduler _foodScheduler;
    handshake::HandshakeHandler _handshake;
};

} // namespace zappy::server

#endif /* !SERVER_SERVER_HPP_ */
