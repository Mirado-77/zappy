/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** A successfully parsed GUI command line
*/

#ifndef PROTOCOL_GUI_PARSEDGUICOMMAND_HPP_
    #define PROTOCOL_GUI_PARSEDGUICOMMAND_HPP_

    #include <vector>

    #include "protocol/gui/GuiCommandKind.hpp"

namespace zappy::protocol::gui {

/**
 * @brief A parsed GUI command: its kind and its list of integer arguments.
 */
struct ParsedGuiCommand {
    GuiCommandKind kind;
    std::vector<int> arguments;
};

/**
 * @brief GUI protocol parsing errors.
 */
enum class GuiParseError {
    UNKNOWN_COMMAND,
    BAD_PARAMETER
};

} // namespace zappy::protocol::gui

#endif /* !PROTOCOL_GUI_PARSEDGUICOMMAND_HPP_ */
