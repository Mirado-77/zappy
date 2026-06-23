/*
** EPITECH PROJECT, 2026
** GuiArgParser.hpp
** File description:
** Parser turning argc/argv into a ServerInfo
*/

// cli::CliParser -> zappy::gui::GuiArgParser

#ifndef GUIARGPARSER_HPP_
    #define GUIARGPARSER_HPP_
    #include <cstddef>
    #include <optional>
    #include <string>
    #include <string_view>
    #include <vector>
    #include "gui/ipc/ServerInfo.hpp"
    #include "ipc/SerializationError.hpp"

namespace zappy::gui {

/**
 * @brief Parses the zappy_gui command line arguments into a @ref ServerInfo.
 */
class GuiArgParser {
public:
    GuiArgParser() = default;

    /**
     * @brief Parse a command line into a validated configuration.
     *
     * @param argc argument count from @c main()
     * @param argv argument vector from @c main()
     * @return std::optional<ipc::ServerInfo> the configuration, or std::nullopt when @c --help was asked
     * @throws SerializationError on any invalid input
     */
    std::optional<ipc::ServerInfo> parse(int argc, const char *const argv[]) const;

    /**
     * @brief The usage / help message.
     *
     * @return std::string the usage string, newline-terminated
     */
    static std::string usage();

private:
    /**
     * @brief Accumulator filled while scanning the argument vector.
     */
    struct Builder {
        std::optional<long> port;
        std::optional<std::string> hostname;
    };

    /**
     * @brief Tokenise the arguments into the builder.
     *
     * @param args raw argument tokens (excluding argv[0])
     * @param out builder to populate
     * @throws CliParseError on any invalid token
     */
    void collect(const std::vector<std::string> &args, Builder &out) const;

    /**
     * @brief Validate the builder and build the config.
     *
     * @param b populated builder
     * @return ServerInfo the validated configuration
     * @throws CliParseError if any required field is missing or invalid
     */
    ipc::ServerInfo finalize(const Builder &b) const;


    /**
     * @brief Take the value token following @p flag, advancing @p index.
     *
     * @param args raw argument tokens
     * @param index in/out: current position in @p args, advanced past the value
     * @param flag the flag being parsed (used in error messages)
     * @return const std::string& reference to the consumed value
     * @throws CliParseError if no value follows the flag
     */
    const std::string &takeValue(const std::vector<std::string> &args,
        std::size_t &index, std::string_view flag) const;

    /**
     * @brief Take an integer flag value, advancing @p index.
     *
     * @param args raw argument tokens
     * @param index in/out: current position in @p args, advanced past the value
     * @param flag the flag being parsed (used in error messages)
     * @return long the parsed integer
     * @throws CliParseError if the value is missing or not a valid integer
     */
    long takeInteger(const std::vector<std::string> &args, std::size_t &index,
    std::string_view flag) const;

};

}

#endif /* GUIARGPARSER_HPP_ */