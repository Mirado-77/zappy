/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Shared utility class to build GUI response lines
*/

#ifndef SERVER_GUI_GUILINEBUILDER_HPP_
    #define SERVER_GUI_GUILINEBUILDER_HPP_

    #include <string>
    #include "server/game/World.hpp"

namespace zappy::server::gui {

/**
 * @brief Helper utility class that centralizes the construction of GUI
 *        protocol response and broadcast strings.
 */
class GuiLineBuilder {
public:
    /**
     * @brief Construct a new Gui Line Builder.
     *
     * @param world read-only reference to the game world
     */
    explicit GuiLineBuilder(const game::World &world) noexcept;

    GuiLineBuilder(const GuiLineBuilder &) = default;
    GuiLineBuilder &operator=(const GuiLineBuilder &) = default;
    GuiLineBuilder(GuiLineBuilder &&) noexcept = default;
    GuiLineBuilder &operator=(GuiLineBuilder &&) noexcept = default;

    ~GuiLineBuilder() = default;

    /**
     * @brief Build the "bct X Y q0..q6" line for tile (X, Y).
     *
     * @param x tile horizontal coordinate
     * @param y tile vertical coordinate
     * @return std::string the formatted line
     */
    [[nodiscard]] std::string bct(int x, int y) const;

    /**
     * @brief Build the "ppo #n X Y O" line for player n.
     *
     * @param playerId player ID
     * @return std::string the formatted line
     */
    [[nodiscard]] std::string ppo(int playerId) const;

    /**
     * @brief Build the "plv #n L" line for player n.
     *
     * @param playerId player ID
     * @return std::string the formatted line
     */
    [[nodiscard]] std::string plv(int playerId) const;

    /**
     * @brief Build the "pin #n X Y q0..q6" line for player n.
     *
     * @param playerId player ID
     * @return std::string the formatted line
     */
    [[nodiscard]] std::string pin(int playerId) const;

private:
    const game::World &_world;
};

} // namespace zappy::server::gui

#endif /* !SERVER_GUI_GUILINEBUILDER_HPP_ */
