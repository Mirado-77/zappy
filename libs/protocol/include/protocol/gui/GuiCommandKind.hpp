/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Closed set of GUI commands understood by the server
*/

#ifndef PROTOCOL_GUI_GUICOMMANDKIND_HPP_
    #define PROTOCOL_GUI_GUICOMMANDKIND_HPP_

    #include <cstddef>

namespace zappy::protocol::gui {

/**
 * @brief Every GUI command, ordered to index the GUI_COMMAND_INFOS table.
 */
enum class GuiCommandKind {
    MSZ,
    BCT,
    MCT,
    TNA,
    PPO,
    PLV,
    PIN,
    SGT,
    SST,
    ADMIN
};

constexpr std::size_t GUI_COMMAND_KIND_COUNT = 10;

} // namespace zappy::protocol::gui

#endif /* !PROTOCOL_GUI_GUICOMMANDKIND_HPP_ */
