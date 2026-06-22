/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Stateless parser turning a raw GUI line into a ParsedGuiCommand
*/

#ifndef PROTOCOL_GUI_GUICOMMANDPARSER_HPP_
    #define PROTOCOL_GUI_GUICOMMANDPARSER_HPP_

    #include <string_view>
    #include <variant>
    #include <vector>

    #include "protocol/gui/GuiCommandInfo.hpp"
    #include "protocol/gui/ParsedGuiCommand.hpp"

namespace zappy::protocol::gui {

/**
 * @brief Parses a GUI command line; pure, stateless,
 * no allocation on failure.
 */
class GuiCommandParser {
public:
    /**
     * @brief Parse @p line into a ParsedGuiCommand or a GuiParseError.
     *
     * Trims surrounding whitespace, splits the verb from its arguments and
     * validates the arguments against the command metadata.
     *
     * @param line raw command line (no trailing newline required)
     * @return std::variant<ParsedGuiCommand, GuiParseError>
     */
    [[nodiscard]] static std::variant<ParsedGuiCommand,
        GuiParseError> parse(std::string_view line);

private:
    /**
     * @brief Strip leading and trailing spaces, tabs and carriage returns.
     *
     * @param s raw view
     * @return std::string_view trimmed view into @p s
     */
    [[nodiscard]] static std::string_view trim(std::string_view s) noexcept;

    /**
     * @brief Split the arguments block into individual tokens.
     *
     * @param rest the trimmed argument block string
     * @return std::vector<std::string_view> argument tokens
     */
    [[nodiscard]] static std::vector<std::string_view> tokenize(
        std::string_view rest);

    /**
     * @brief Parse tokens into integers, checking for non-negativity.
     *
     * @param tokens individual tokens
     * @param args target vector to populate
     * @return bool true if all tokens parsed successfully as non-negative
     */
    [[nodiscard]] static bool convertArgs(
        const std::vector<std::string_view> &tokens,
        std::vector<int> &args) noexcept;

    /**
     * @brief Construct the ParsedGuiCommand.
     *
     * @param kind the command kind
     * @param tokens the argument tokens
     * @return std::variant<ParsedGuiCommand, GuiParseError>
     */
    [[nodiscard]] static std::variant<ParsedGuiCommand,
        GuiParseError> buildCommand(GuiCommandKind kind,
            const std::vector<std::string_view> &tokens);

    /**
     * @brief Build a command whose payload is a raw, unparsed string argument.
     *
     * @param kind the command kind
     * @param rest the trimmed argument block (the raw payload)
     * @return std::variant<ParsedGuiCommand, GuiParseError> BAD_PARAMETER if empty
     */
    [[nodiscard]] static std::variant<ParsedGuiCommand,
        GuiParseError> buildRawCommand(GuiCommandKind kind,
            std::string_view rest);

    /**
     * @brief Resolve a matched command into a parse result.
     *
     * Routes raw-argument commands to buildRawCommand and integer-argument
     * commands through tokenize + arity validation + buildCommand.
     *
     * @param info matched command metadata
     * @param kind the command kind
     * @param rest the trimmed argument block
     * @return std::variant<ParsedGuiCommand, GuiParseError>
     */
    [[nodiscard]] static std::variant<ParsedGuiCommand,
        GuiParseError> resolve(const GuiCommandInfo &info,
            GuiCommandKind kind, std::string_view rest);
};

} // namespace zappy::protocol::gui

#endif /* !PROTOCOL_GUI_GUICOMMANDPARSER_HPP_ */
