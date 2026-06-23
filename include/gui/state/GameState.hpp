/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GUI-side mirror of the server world
*/

#ifndef GUI_STATE_GAMESTATE_HPP_
    #define GUI_STATE_GAMESTATE_HPP_

    #include <array>
    #include <cstddef>
    #include <string>
    #include <unordered_map>
    #include <vector>

    #include "gui/net/ServerEvent.hpp"

namespace zappy::gui::state {

constexpr std::size_t RESOURCE_COUNT = 7;

using Resources = std::array<int, RESOURCE_COUNT>;

enum class Orientation { NORTH = 1, EAST = 2, SOUTH = 3, WEST = 4 };

struct Tile {
    int x = 0;
    int y = 0;
    Resources res{};
};

struct Player {
    int id = 0;
    int x = 0;
    int y = 0;
    Orientation orient = Orientation::NORTH;
    int level = 1;
    std::string team;
    Resources inventory{};
    bool alive = true;
};

struct Egg {
    int id = 0;
    int player_id = -1;
    int x = 0;
    int y = 0;
    bool alive = true;
};

class GameState {
public:
    void apply(const net::ServerEvent &ev);

    [[nodiscard]] int width() const noexcept { return _width; }
    [[nodiscard]] int height() const noexcept { return _height; }
    [[nodiscard]] int time_unit() const noexcept { return _time_unit; }
    [[nodiscard]] const Tile &tile(int x, int y) const;
    [[nodiscard]] const std::unordered_map<int, Player> &players() const
        noexcept { return _players; }
    [[nodiscard]] const std::unordered_map<int, Egg> &eggs() const
        noexcept { return _eggs; }
    [[nodiscard]] const std::vector<std::string> &teams() const
        noexcept { return _teams; }
    [[nodiscard]] bool game_ended() const noexcept { return _ended; }
    [[nodiscard]] const std::string &winner() const noexcept { return _winner; }

private:
    void on_msz(const net::ServerEvent &);
    void on_sgt(const net::ServerEvent &);
    void on_tna(const net::ServerEvent &);
    void on_bct(const net::ServerEvent &);
    void on_pnw(const net::ServerEvent &);
    void on_ppo(const net::ServerEvent &);
    void on_plv(const net::ServerEvent &);
    void on_pin(const net::ServerEvent &);
    void on_pdi(const net::ServerEvent &);
    void on_enw(const net::ServerEvent &);
    void on_edi(const net::ServerEvent &);
    void on_seg(const net::ServerEvent &);

    void resize_tiles();
    [[nodiscard]] std::size_t idx(int x, int y) const noexcept
        { return static_cast<std::size_t>(y) * _width + x; }

    int _width = 0;
    int _height = 0;
    int _time_unit = 100;
    std::vector<Tile> _tiles;
    std::unordered_map<int, Player> _players;
    std::unordered_map<int, Egg> _eggs;
    std::vector<std::string> _teams;
    bool _ended = false;
    std::string _winner;
};

} // namespace zappy::gui::state

#endif /* !GUI_STATE_GAMESTATE_HPP_ */