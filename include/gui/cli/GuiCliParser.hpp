/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GUI command line parser
*/

#ifndef GUI_CLI_GUICLIPARSER_HPP_
    #define GUI_CLI_GUICLIPARSER_HPP_

    #include <cstdint>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <string_view>

namespace zappy::gui::cli {

struct GuiOptions {
    std::uint16_t port;
    std::string host;
};

class GuiCliError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class GuiCliParser {
public:
    [[nodiscard]] static GuiOptions parse(int argc, char **argv);
    static void print_usage(std::ostream &os, std::string_view prog);

private:
    static bool is_help_flag(std::string_view arg) noexcept;
    [[nodiscard]] static std::uint16_t parse_port(std::string_view raw);
    [[nodiscard]] static std::string parse_host(std::string_view raw);
};

} // namespace zappy::gui::cli

#endif /* !GUI_CLI_GUICLIPARSER_HPP_ */