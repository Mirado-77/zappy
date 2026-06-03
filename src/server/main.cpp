/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** zappy_server entry point: parse the CLI, then boot the server
*/

#include <iostream>
#include <optional>

#include "server/Constants.hpp"
#include "server/cli/CliParseError.hpp"
#include "server/cli/CliParser.hpp"
#include "server/cli/ServerConfig.hpp"

int main(int argc, char **argv)
{
    int status = zappy::server::ExitSuccess;

    try {
        zappy::server::cli::CliParser parser;
        std::optional<zappy::server::cli::ServerConfig> config =
            parser.parse(argc, argv);

        if (!config) {
            std::cout << zappy::server::cli::CliParser::usage();
        } else {
            std::cout << "zappy_server: configuration parsed (port "
                      << config->port() << ", world " << config->width() << "x"
                      << config->height() << ", " << config->teamNames().size()
                      << " team(s))\n";
        }
    } catch (const zappy::server::cli::CliParseError &error) {
        std::cerr << "Error: " << error.what() << "\n";
        std::cerr << zappy::server::cli::CliParser::usage();
        status = zappy::server::ExitFailure;
    }
    return status;
}
