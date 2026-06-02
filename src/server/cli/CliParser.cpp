/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Implementation of the zappy_server command line parser
*/

#include "server/cli/CliParser.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include "server/cli/CliParseError.hpp"

namespace zappy::server::cli {

namespace {

constexpr int kDefaultFrequency = 100;
constexpr std::string_view kReservedTeamName = "GRAPHIC";

/// @brief Parse a whole token as a base-10 integer; reject any extra char.
long parseStrictInteger(const std::string &token, std::string_view flag)
{
    std::size_t consumed = 0;
    long value = 0;

    try {
        value = std::stol(token, &consumed);
    } catch (const std::exception &) {
        consumed = 0;
    }
    if (consumed == 0 || consumed != token.size())
        throw CliParseError("invalid integer for " + std::string(flag)
            + ": '" + token + "'");
    return value;
}

/// @brief Require a positive value (> 0) for a flag, or throw.
void requirePositive(long value, std::string_view flag)
{
    if (value <= 0)
        throw CliParseError(std::string(flag) + " must be strictly positive");
}

} // namespace

const std::string &CliParser::takeValue(const std::vector<std::string> &args,
    std::size_t &index, std::string_view flag) const
{
    if (index + 1 >= args.size())
        throw CliParseError("missing value for " + std::string(flag));
    return args[++index];
}

long CliParser::takeInteger(const std::vector<std::string> &args,
    std::size_t &index, std::string_view flag) const
{
    return parseStrictInteger(takeValue(args, index, flag), flag);
}

void CliParser::takeTeamNames(const std::vector<std::string> &args,
    std::size_t &index, Builder &out) const
{
    while (index + 1 < args.size() && args[index + 1][0] != '-') {
        const std::string &name = args[++index];
        if (name.empty())
            throw CliParseError("team name cannot be empty");
        out.teamNames.push_back(name);
    }
}

void CliParser::collect(const std::vector<std::string> &args, Builder &out) const
{
    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string &arg = args[i];

        if (arg == "-p")
            out.port = takeInteger(args, i, arg);
        else if (arg == "-x")
            out.width = takeInteger(args, i, arg);
        else if (arg == "-y")
            out.height = takeInteger(args, i, arg);
        else if (arg == "-c")
            out.clientsPerTeam = takeInteger(args, i, arg);
        else if (arg == "-f")
            out.frequency = takeInteger(args, i, arg);
        else if (arg == "-n")
            takeTeamNames(args, i, out);
        else if (arg == "--enable-events")
            out.enableEvents = true;
        else if (arg == "--enable-biomes")
            out.enableBiomes = true;
        else if (arg == "--enable-admin")
            out.enableAdmin = true;
        else if (arg == "--admin-password")
            out.adminPassword = takeValue(args, i, arg);
        else
            throw CliParseError("unknown argument: " + arg);
    }
}

ServerConfig CliParser::finalize(const Builder &b) const
{
    if (!b.port)
        throw CliParseError("missing required argument: -p");
    if (b.port.value() <= 0
        || b.port.value() > std::numeric_limits<std::uint16_t>::max())
        throw CliParseError("-p port must be in range 1..65535");

    if (!b.width)
        throw CliParseError("missing required argument: -x");
    requirePositive(b.width.value(), "-x");

    if (!b.height)
        throw CliParseError("missing required argument: -y");
    requirePositive(b.height.value(), "-y");

    if (b.teamNames.empty())
        throw CliParseError("missing required argument: -n (at least one team)");
    for (const std::string &name : b.teamNames) {
        if (name == kReservedTeamName)
            throw CliParseError("team name 'GRAPHIC' is reserved");
        if (std::count(b.teamNames.begin(), b.teamNames.end(), name) > 1)
            throw CliParseError("duplicate team name: " + name);
    }

    if (!b.clientsPerTeam)
        throw CliParseError("missing required argument: -c");
    requirePositive(b.clientsPerTeam.value(), "-c");

    const long frequency = b.frequency.value_or(kDefaultFrequency);
    requirePositive(frequency, "-f");

    return ServerConfig(static_cast<std::uint16_t>(b.port.value()),
        static_cast<int>(b.width.value()), static_cast<int>(b.height.value()),
        b.teamNames, static_cast<int>(b.clientsPerTeam.value()),
        static_cast<int>(frequency), b.enableEvents, b.enableBiomes,
        b.enableAdmin, b.adminPassword.value_or(std::string{}));
}

std::optional<ServerConfig> CliParser::parse(int argc,
    const char *const argv[]) const
{
    std::optional<ServerConfig> result;
    std::vector<std::string> args;
    bool helpRequested = false;

    args.reserve(static_cast<std::size_t>(argc < 0 ? 0 : argc));
    for (int i = 0; i < argc; ++i)
        args.emplace_back(argv[i]);
    for (const std::string &arg : args) {
        if (arg == "--help")
            helpRequested = true;
    }
    if (!helpRequested) {
        Builder builder;
        collect(args, builder);
        result.emplace(finalize(builder));
    }
    return result;
}

std::string CliParser::usage()
{
    return
        "USAGE: ./zappy_server -p port -x width -y height -n name1 name2 ... "
        "-c clientsNb -f freq\n"
        "-p port      port number\n"
        "-x width     width of the world\n"
        "-y height    height of the world\n"
        "-n name1 ... name of the team\n"
        "-c clientsNb number of authorized clients per team\n"
        "-f freq      reciprocal of time unit for execution of actions\n"
        "--enable-events       (optional) enable dynamic events [bonus]\n"
        "--enable-biomes       (optional) enable biome generation [bonus]\n"
        "--enable-admin        (optional) enable admin console [bonus]\n"
        "--admin-password <pw> (optional) admin console password [bonus]\n";
}

} // namespace zappy::server::cli
