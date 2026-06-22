/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GuiCommandParser implementation
*/

#include <algorithm>
#include <charconv>
#include <string>
#include <system_error>

#include "protocol/gui/GuiCommandInfo.hpp"
#include "protocol/gui/GuiCommandParser.hpp"

/**
 * @brief Trim leading and trailing spaces, tabs and carriage returns.
 *
 * @param s raw view
 * @return std::string_view trimmed view
 */
std::string_view zappy::protocol::gui::GuiCommandParser::trim(
    std::string_view s) noexcept
{
    const std::size_t first = s.find_first_not_of(" \t\r");
    const std::size_t last = s.find_last_not_of(" \t\r");
    return first == std::string_view::npos
        ? std::string_view{} : s.substr(first, last - first + 1);
}

/**
 * @brief Split the arguments block into individual tokens.
 *
 * @param rest the trimmed argument block string
 * @return std::vector<std::string_view> argument tokens
 */
std::vector<std::string_view> zappy::protocol::gui::GuiCommandParser::tokenize(
    std::string_view rest)
{
    std::vector<std::string_view> tokens;
    std::size_t start = rest.find_first_not_of(" \t\r");
    while (start != std::string_view::npos) {
        const std::size_t end = rest.find_first_of(" \t\r", start);
        tokens.push_back(end == std::string_view::npos
            ? rest.substr(start) : rest.substr(start, end - start));
        start = end == std::string_view::npos
            ? std::string_view::npos : rest.find_first_not_of(" \t\r", end);
    }
    return tokens;
}

/**
 * @brief Parse tokens into integers, checking for non-negativity.
 *
 * @param tokens individual tokens
 * @param args target vector to populate
 * @return bool true if all tokens parsed successfully as non-negative
 */
bool zappy::protocol::gui::GuiCommandParser::convertArgs(
    const std::vector<std::string_view> &tokens,
    std::vector<int> &args) noexcept
{
    bool ok = true;
    for (const auto &token : tokens) {
        int val = 0;
        const std::string_view num = (!token.empty() && token.front() == '#')
            ? token.substr(1) : token;
        const auto [ptr, ec] = std::from_chars(
            num.data(), num.data() + num.size(), val);
        if (ec != std::errc{} || ptr != num.data() + num.size() || val < 0)
            ok = false;
        else
            args.push_back(val);
    }
    return ok;
}

/**
 * @brief Construct the ParsedGuiCommand.
 *
 * @param kind the command kind
 * @param tokens the argument tokens
 * @return std::variant<ParsedGuiCommand, GuiParseError>
 */
std::variant<zappy::protocol::gui::ParsedGuiCommand,
    zappy::protocol::gui::GuiParseError>
zappy::protocol::gui::GuiCommandParser::buildCommand(
    GuiCommandKind kind, const std::vector<std::string_view> &tokens)
{
    std::vector<int> args;
    std::variant<ParsedGuiCommand, GuiParseError> result =
        GuiParseError::BAD_PARAMETER;

    args.reserve(tokens.size());
    if (convertArgs(tokens, args))
        result = ParsedGuiCommand{kind, std::move(args), std::string{}};
    return result;
}

/**
 * @brief Build a command whose payload is a raw, unparsed string argument.
 *
 * @param kind the command kind
 * @param rest the trimmed argument block (the raw payload)
 * @return std::variant<ParsedGuiCommand, GuiParseError> BAD_PARAMETER if empty
 */
std::variant<zappy::protocol::gui::ParsedGuiCommand,
    zappy::protocol::gui::GuiParseError>
zappy::protocol::gui::GuiCommandParser::buildRawCommand(
    GuiCommandKind kind, std::string_view rest)
{
    std::variant<ParsedGuiCommand, GuiParseError> result =
        GuiParseError::BAD_PARAMETER;

    if (!rest.empty())
        result = ParsedGuiCommand{kind, {}, std::string(rest)};
    return result;
}

/**
 * @brief Resolve a matched command into a parse result.
 *
 * @param info matched command metadata
 * @param kind the command kind
 * @param rest the trimmed argument block
 * @return std::variant<ParsedGuiCommand, GuiParseError>
 */
std::variant<zappy::protocol::gui::ParsedGuiCommand,
    zappy::protocol::gui::GuiParseError>
zappy::protocol::gui::GuiCommandParser::resolve(
    const GuiCommandInfo &info, GuiCommandKind kind, std::string_view rest)
{
    std::variant<ParsedGuiCommand, GuiParseError> result =
        GuiParseError::BAD_PARAMETER;
    std::vector<std::string_view> tokens;

    if (info.acceptsRawArgument)
        result = buildRawCommand(kind, rest);
    else {
        tokens = tokenize(rest);
        if (tokens.size() == static_cast<std::size_t>(info.argCount))
            result = buildCommand(kind, tokens);
    }
    return result;
}

/**
 * @brief Parse a GUI command line.
 *
 * @param line raw command line
 * @return std::variant<ParsedGuiCommand, GuiParseError>
 */
std::variant<zappy::protocol::gui::ParsedGuiCommand,
    zappy::protocol::gui::GuiParseError>
zappy::protocol::gui::GuiCommandParser::parse(std::string_view line)
{
    const std::string_view trimmed = trim(line);
    std::variant<ParsedGuiCommand, GuiParseError> result =
        GuiParseError::UNKNOWN_COMMAND;
    const std::size_t sep = trimmed.find_first_of(" \t\r");
    const std::string_view verb = trimmed.substr(0, sep);
    const std::string_view rest = sep == std::string_view::npos
        ? std::string_view{} : trim(trimmed.substr(sep));
    const auto it = std::find_if(GUI_COMMAND_INFOS.begin(),
        GUI_COMMAND_INFOS.end(), [verb](const GuiCommandInfo &info) noexcept {
            return info.name == verb;
        });

    if (!trimmed.empty() && it != GUI_COMMAND_INFOS.end())
        result = resolve(*it,
            static_cast<GuiCommandKind>(it - GUI_COMMAND_INFOS.begin()), rest);
    return result;
}
