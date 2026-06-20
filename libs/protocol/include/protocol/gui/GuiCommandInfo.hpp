/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Static metadata for every GUI command (name, argument count)
*/

#ifndef PROTOCOL_GUI_GUICOMMANDINFO_HPP_
    #define PROTOCOL_GUI_GUICOMMANDINFO_HPP_

    #include <array>
    #include <cstddef>
    #include <string_view>

    #include "protocol/gui/GuiCommandKind.hpp"

namespace zappy::protocol::gui {

/**
 * @brief Static description of a single GUI command.
 */
struct GuiCommandInfo {
    std::string_view name;
    int argCount;

    /**
     * @brief Table entry for @p kind.
     *
     * @param kind command to describe
     * @return const GuiCommandInfo& the matching entry
     */
    [[nodiscard]] static constexpr const GuiCommandInfo &guiCommandInfoFor(
        GuiCommandKind kind) noexcept;
};

inline constexpr std::array<GuiCommandInfo,
    GUI_COMMAND_KIND_COUNT> GUI_COMMAND_INFOS = {{
    {"msz", 0},
    {"bct", 2},
    {"mct", 0},
    {"tna", 0},
    {"ppo", 1},
    {"plv", 1},
    {"pin", 1},
    {"sgt", 0},
    {"sst", 1}
}};

constexpr const GuiCommandInfo &GuiCommandInfo::guiCommandInfoFor(
    GuiCommandKind kind) noexcept
{
    return GUI_COMMAND_INFOS[static_cast<std::size_t>(kind)];
}

} // namespace zappy::protocol::gui

#endif /* !PROTOCOL_GUI_GUICOMMANDINFO_HPP_ */
