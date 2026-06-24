/*
** EPITECH PROJECT, 2026
** BoardData.hpp
** File description:
** BoardData
*/

#ifndef BOARDDATA_HPP_
    #define BOARDDATA_HPP_
    #include <cstddef>
    #include <string>
    #include <unordered_map>
    #include <vector>

    #include "gui/board_data/GraphicalEvent.hpp"
    #include "gui/board_data/TileData.hpp"
    #include "gui/ipc/ServerInfo.hpp"
    #include "gui/net/ServerEvent.hpp"

namespace zappy::gui::board_data {

struct PlayerData {
    int id = 0;
    int x = 0;
    int y = 0;
    Orientation orient = Orientation::NORTH;
    int level = 1;
    std::string team;
    Resources inventory{};
    bool alive = true;
};

struct EggData {
    int id = 0;
    int player_id = -1;
    int x = 0;
    int y = 0;
    bool alive = true;
};

class BoardData {
public:
    explicit BoardData(const ipc::ServerInfo &server_info);

    void apply(const net::ServerEvent &ev);

    [[nodiscard]] int width() const noexcept { return _width; }
    [[nodiscard]] int height() const noexcept { return _height; }
    [[nodiscard]] int time_unit() const noexcept { return _time_unit; }
    [[nodiscard]] TileData &get_tile_value(std::size_t x, std::size_t y);
    [[nodiscard]] const std::unordered_map<int, PlayerData> &players()
        const noexcept { return _players; }
    [[nodiscard]] const std::unordered_map<int, EggData> &eggs()
        const noexcept { return _eggs; }
    [[nodiscard]] const std::vector<std::string> &teams()
        const noexcept { return _teams; }
    [[nodiscard]] const std::vector<GraphicalEvent> &events()
        const noexcept { return _events; }
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
    void on_pbc(const net::ServerEvent &);
    void on_pic(const net::ServerEvent &);
    void on_pie(const net::ServerEvent &);
    void on_pex(const net::ServerEvent &);
    void on_pfk(const net::ServerEvent &);

    void resize_tiles();
    void push_player_event(GraphicalEventKind kind, int player_id,
        std::string text);
    [[nodiscard]] std::size_t idx(int x, int y) const noexcept
        { return static_cast<std::size_t>(y) * _width + x; }

    ipc::ServerInfo _server_info;
    int _width = 0;
    int _height = 0;
    int _time_unit = 100;
    std::vector<TileData> _tiles;
    std::unordered_map<int, PlayerData> _players;
    std::unordered_map<int, EggData> _eggs;
    std::vector<std::string> _teams;
    std::vector<GraphicalEvent> _events;
    bool _ended = false;
    std::string _winner;
};

} // namespace zappy::gui::board_data

#endif /* BOARDDATA_HPP_ */
