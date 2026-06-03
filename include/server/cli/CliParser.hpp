/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Command line parser turning argc/argv into an ServerConfig
*/

#ifndef CLI_CLIPARSER_HPP_
    #define CLI_CLIPARSER_HPP_

    #include <cstddef>
    #include <optional>
    #include <string>
    #include <string_view>
    #include <vector>

    #include "server/cli/ServerConfig.hpp"

namespace zappy::server::cli {

/**
 * @brief Parses the zappy_server command line into a @ref ServerConfig.
 */
class CliParser {
public:
    CliParser() = default;

    /**
     * @brief Parse a command line into a validated configuration.
     * @param argc Argument count from @c main().
     * @param argv Argument vector from @c main().
     * @return The configuration, or @c std::nullopt when @c --help was asked.
     * @throws CliParseError on any invalid input.
     */
    std::optional<ServerConfig> parse(int argc, const char *const argv[]) const;

    /**
     * @brief The usage / help message.
     * @return The usage string, newline-terminated.
     */
    static std::string usage();

private:
    /// @brief Accumulator filled while scanning the argument vector.
    struct Builder {
        std::optional<long> port;
        std::optional<long> width;
        std::optional<long> height;
        std::vector<std::string> teamNames;
        std::optional<long> clientsPerTeam;
        std::optional<long> frequency;

        bool enableEvents = false;
        bool enableBiomes = false;
        bool enableAdmin = false;
        std::optional<std::string> adminPassword;
    };

    /// @brief Tokenise the arguments into the builder.
    void collect(const std::vector<std::string> &args, Builder &out) const;

    /// @brief Validate the builder and build the config.
    ServerConfig finalize(const Builder &b) const;

    /// @brief Take the value token following @p flag, advancing @p index.
    const std::string &takeValue(const std::vector<std::string> &args,
        std::size_t &index, std::string_view flag) const;

    /// @brief Take an integer flag value, advancing @p index.
    long takeInteger(const std::vector<std::string> &args, std::size_t &index,
        std::string_view flag) const;

    /// @brief Take a -n run of team names, advancing @p index.
    void takeTeamNames(const std::vector<std::string> &args, std::size_t &index,
        Builder &out) const;
};

}

#endif /* !CLI_CLIPARSER_HPP_ */
