/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GUI command line parser implementation
*/

#include "gui/cli/GuiCliParser.hpp"

#include <charconv>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <iostream>

namespace zappy::gui::cli {

namespace {

constexpr std::string_view OPT_PORT = "-p";
constexpr std::string_view OPT_HOST = "-h";

[[noreturn]] void fail(const std::string &reason)
{
    throw GuiCliError(reason);
}

std::string_view require_value(int argc, char **argv, int &i,
    std::string_view opt)
{
    if (i + 1 >= argc)
        fail(std::string("missing value for option ") + std::string(opt));
    ++i;
    return argv[i];
}

} // namespace

bool GuiCliParser::is_help_flag(std::string_view arg) noexcept
{
    return arg == "--help" || arg == "-help";
}

std::uint16_t GuiCliParser::parse_port(std::string_view raw)
{
    if (raw.empty())
        fail("port value is empty");

    int value = 0;
    auto [ptr, ec] =
        std::from_chars(raw.data(), raw.data() + raw.size(), value);

    if (ec != std::errc{} || ptr != raw.data() + raw.size())
        fail("port must be a positive integer");
    if (value < 1 || value > 65535)
        fail("port must be in [1, 65535]");
    return static_cast<std::uint16_t>(value);
}

std::string GuiCliParser::parse_host(std::string_view raw)
{
    if (raw.empty())
        fail("host value is empty");
    return std::string(raw);
}

GuiOptions GuiCliParser::parse(int argc, char **argv)
{
    std::optional<std::uint16_t> port;
    std::optional<std::string> host;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];

        if (is_help_flag(arg)) {
            print_usage(std::cout, argv[0]);
            std::exit(0);
        }
        if (arg == OPT_PORT) {
            if (port.has_value())
                fail("option -p specified twice");
            port = parse_port(require_value(argc, argv, i, OPT_PORT));
            continue;
        }
        if (arg == OPT_HOST) {
            if (host.has_value())
                fail("option -h specified twice");
            host = parse_host(require_value(argc, argv, i, OPT_HOST));
            continue;
        }
        fail(std::string("unknown option: ") + std::string(arg));
    }

    if (!port.has_value())
        fail("missing required option -p");
    if (!host.has_value())
        fail("missing required option -h");

    return GuiOptions{*port, std::move(*host)};
}

void GuiCliParser::print_usage(std::ostream &os, std::string_view prog)
{
    os << "USAGE: " << prog << " -p port -h machine\n"
       << "    port      is the port number\n"
       << "    machine   is the name of the machine; localhost by default\n";
}

} // namespace zappy::gui::cli