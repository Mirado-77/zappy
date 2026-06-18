/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GuiDispatcher implementation
*/

#include <utility>

#include "server/gui/GuiDispatcher.hpp"

/**
 * @brief Construct a new Gui Dispatcher object.
 *
 * @param world the game world reference
 * @param onResponse the callback to dispatch responses
 */
zappy::server::gui::GuiDispatcher::GuiDispatcher(
    game::World &world,
    std::function<void(int fd, std::string)> onResponse)
    : _world(world), _onResponse(std::move(onResponse))
{
}

/**
 * @brief Dispatch a raw command line from a specific client.
 *
 * @param fd client file descriptor
 * @param line raw command line
 */
void zappy::server::gui::GuiDispatcher::dispatch(
    int fd, std::string_view line)
{
    static constexpr std::array<Handler,
        protocol::gui::GUI_COMMAND_KIND_COUNT> handlers = {
            &GuiDispatcher::handleMsz, &GuiDispatcher::handleBct,
            &GuiDispatcher::handleMct, &GuiDispatcher::handleTna,
            &GuiDispatcher::handlePpo, &GuiDispatcher::handlePlv,
            &GuiDispatcher::handlePin, &GuiDispatcher::handleSgt,
            &GuiDispatcher::handleSst};
    const auto res = protocol::gui::GuiCommandParser::parse(line);

    if (std::holds_alternative<protocol::gui::GuiParseError>(res)) {
        sendError(fd, std::get<protocol::gui::GuiParseError>(res));
    } else {
        const auto cmd = std::get<protocol::gui::ParsedGuiCommand>(res);
        const std::size_t idx = static_cast<std::size_t>(cmd.kind);
        const std::string response = (this->*handlers[idx])(cmd.arguments);
        if (!response.empty())
            _onResponse(fd, response);
    }
}

/**
 * @brief Send appropriate error message to the client on parse error.
 *
 * @param fd client file descriptor
 * @param err type of parsing error encountered
 */
void zappy::server::gui::GuiDispatcher::sendError(
    int fd, protocol::gui::GuiParseError err)
{
    const bool isUnknown =
        (err == protocol::gui::GuiParseError::UNKNOWN_COMMAND);
    _onResponse(fd, isUnknown ? "suc" : "sbp");
}

/**
 * @brief Handle msz command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleMsz(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-msz";
}

/**
 * @brief Handle bct command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleBct(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-bct";
}

/**
 * @brief Handle mct command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleMct(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-mct";
}

/**
 * @brief Handle tna command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleTna(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-tna";
}

/**
 * @brief Handle ppo command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePpo(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-ppo";
}

/**
 * @brief Handle plv command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePlv(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-plv";
}

/**
 * @brief Handle pin command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handlePin(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-pin";
}

/**
 * @brief Handle sgt command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleSgt(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-sgt";
}

/**
 * @brief Handle sst command.
 *
 * @param args command arguments
 * @return std::string response string
 */
std::string zappy::server::gui::GuiDispatcher::handleSst(
    const std::vector<int> &args)
{
    (void)args;
    return "stub-sst";
}
