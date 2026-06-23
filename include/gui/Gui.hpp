/*
** EPITECH PROJECT, 2026
** Gui.hpp
** File description:
** Top-level gui orchestrator
*/

#ifndef GUI_HPP_
    #define GUI_HPP_
    #include "gui/ipc/ServerInfo.hpp"
/*
    #include "server/ai/AiDispatcher.hpp"
    #include "server/gui/GuiDispatcher.hpp"
    #include "server/gui/GuiNotifier.hpp"
    #include "server/config/FeatureFlags.hpp"
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
*/

namespace zappy::gui {

/**
 * @brief Top-level orchestrator owning the listener, poll loop and client registry.
 */
class Gui {
public:
    /**
     * @brief Build the gui from a validated server info.
     *
     * @param config the parsed CLI configuration
     */
    explicit Gui(const ipc::ServerInfo &server_info);
    ~Gui();

    Gui(const Gui &) = delete;
    Gui &operator=(const Gui &) = delete;

    /**
     * @brief Run the event loop until SIGINT.
     */
    void run();

private:
    /**
     * @brief Read lines from standard input and broadcast them to all GUI clients.
     */
    void pullServerInfo();

    /**
     * @brief pull the server info and start the window
     */
    void init();

    /**
     * @brief pull the server info and then display
     */
    void update();

    /**
     * @brief conect to the server
     */
    void connectServer();

    /**
     * @brief pull events from server and dispatch them
     */
    void handleServerEvents();

    /**
     * @brief Broadcast a message to the server.
     */
    void Broadcast();

    ipc::ServerInfo _server_info;
/*
    net::Listener _listener;
    net::PollLoop _poll;
    game::World _world;
    scheduler::SteadyClock _clock;
    scheduler::Scheduler _scheduler{_clock};
    int _frequency;
    handshake::HandshakeHandler _handshake;
*/
};

} // namespace zappy::server

#endif /* GUI_HPP_ */
