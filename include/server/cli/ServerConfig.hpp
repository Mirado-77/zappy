/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Server configuration produced by the CLI parser
*/

#ifndef CLI_SERVERCONFIG_HPP_
    #define CLI_SERVERCONFIG_HPP_

    #include <cstdint>
    #include <string>
    #include <utility>
    #include <vector>

namespace zappy::server::cli {
class ServerConfig {
public:
    /**
     * @brief Construct a configuration from validated values.
     * @param port Listening port (1..65535).
     * @param width World width in tiles (> 0).
     * @param height World height in tiles (> 0).
     * @param teamNames Unique team names (at least one, no "GRAPHIC").
     * @param clientsPerTeam Authorised clients per team (> 0).
     * @param frequency Reciprocal of the time unit (> 0).
     * @param enableEvents Bonus: dynamic events.
     * @param enableBiomes Bonus: biome generation.
     * @param enableAdmin  Bonus: admin console.
     * @param adminPassword Bonus: admin console password.
     */
    ServerConfig(std::uint16_t port, int width, int height,
        std::vector<std::string> teamNames, int clientsPerTeam, int frequency,
        bool enableEvents = false, bool enableBiomes = false,
        bool enableAdmin = false, std::string adminPassword = {})
        : _port(port), _width(width), _height(height),
        _teamNames(std::move(teamNames)), _clientsPerTeam(clientsPerTeam),
        _frequency(frequency), _enableEvents(enableEvents),
        _enableBiomes(enableBiomes), _enableAdmin(enableAdmin),
        _adminPassword(std::move(adminPassword))
    {
    }

    /// @return Listening port number.
    std::uint16_t port() const noexcept { return _port; }
    
    /// @return World width in tiles.
    int width() const noexcept { return _width; }
    
    /// @return World height in tiles.
    int height() const noexcept { return _height; }
    
    /// @return Immutable list of unique team names.
    const std::vector<std::string> &teamNames() const noexcept { return _teamNames; }
    
    /// @return Initial authorised clients per team.
    int clientsPerTeam() const noexcept { return _clientsPerTeam; }
    
    /// @return Reciprocal of the time unit for action execution.
    int frequency() const noexcept { return _frequency; }

    
    /// @return Whether the bonus events subsystem is enabled.
    bool eventsEnabled() const noexcept { return _enableEvents; }
    
    /// @return Whether the bonus biomes subsystem is enabled.
    bool biomesEnabled() const noexcept { return _enableBiomes; }
    
    /// @return Whether the bonus admin subsystem is enabled.
    bool adminEnabled() const noexcept { return _enableAdmin; }

    /// @return Configured admin password (empty if none provided).
    const std::string &adminPassword() const noexcept { return _adminPassword; }

private:
    const std::uint16_t _port;
    const int _width;
    const int _height;
    const std::vector<std::string> _teamNames;
    const int _clientsPerTeam;
    const int _frequency;

    const bool _enableEvents;
    const bool _enableBiomes;
    const bool _enableAdmin;
    const std::string _adminPassword;
};

}
#endif /* !CLI_SERVERCONFIG_HPP_ */
