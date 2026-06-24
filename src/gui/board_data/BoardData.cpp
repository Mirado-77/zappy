/*
** EPITECH PROJECT, 2026
** BoardData.cpp
** File description:
** BoardData
*/

#include "gui/board_data/BoardData.hpp"

#include <stdexcept>
#include <utility>

namespace zappy::gui::board_data {

BoardData::BoardData(const ipc::ServerInfo &server_info)
    : _server_info(server_info)
{
}

void BoardData::apply(const net::ServerEvent &ev)
{
    using K = net::EventKind;
    switch (ev.kind) {
        case K::MSZ: on_msz(ev); break;
        case K::SGT: case K::SST: on_sgt(ev); break;
        case K::TNA: on_tna(ev); break;
        case K::BCT: on_bct(ev); break;
        case K::PNW: on_pnw(ev); break;
        case K::PPO: on_ppo(ev); break;
        case K::PLV: on_plv(ev); break;
        case K::PIN: on_pin(ev); break;
        case K::PDI: on_pdi(ev); break;
        case K::ENW: on_enw(ev); break;
        case K::EDI: on_edi(ev); break;
        case K::SEG: on_seg(ev); break;
        case K::PBC: on_pbc(ev); break;
        case K::PIC: on_pic(ev); break;
        case K::PIE: on_pie(ev); break;
        case K::PEX: on_pex(ev); break;
        case K::PFK: on_pfk(ev); break;
        default: break;
    }
}

TileData &BoardData::get_tile_value(std::size_t x, std::size_t y)
{
    if (static_cast<int>(x) >= _width || static_cast<int>(y) >= _height)
        throw std::out_of_range("get_tile_value() out of bounds");
    return _tiles[y * static_cast<std::size_t>(_width) + x];
}

void BoardData::resize_tiles()
{
    _tiles.assign(static_cast<std::size_t>(_width) * _height, TileData{});
    for (int y = 0; y < _height; ++y)
        for (int x = 0; x < _width; ++x)
            _tiles[idx(x, y)] = TileData{x, y, {}};
}

void BoardData::push_player_event(GraphicalEventKind kind, int player_id,
    std::string text)
{
    auto it = _players.find(player_id);
    GraphicalEvent ev;
    ev.kind = kind;
    ev.player_id = player_id;
    ev.text = std::move(text);
    if (it != _players.end()) {
        ev.x = it->second.x;
        ev.y = it->second.y;
    }
    _events.push_back(std::move(ev));
}

void BoardData::on_msz(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 2)
        return;
    _width = ev.ints[0];
    _height = ev.ints[1];
    resize_tiles();
}

void BoardData::on_sgt(const net::ServerEvent &ev)
{
    if (!ev.ints.empty())
        _time_unit = ev.ints[0];
}

void BoardData::on_tna(const net::ServerEvent &ev)
{
    if (!ev.text.empty())
        _teams.push_back(ev.text);
}

void BoardData::on_bct(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 9)
        return;
    const int x = ev.ints[0];
    const int y = ev.ints[1];
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    TileData &t = _tiles[idx(x, y)];
    t.x = x;
    t.y = y;
    for (std::size_t i = 0; i < RESOURCE_COUNT; ++i)
        t.res[i] = ev.ints[2 + i];
}

void BoardData::on_pnw(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 5)
        return;
    PlayerData p;
    p.id = ev.ints[0];
    p.x = ev.ints[1];
    p.y = ev.ints[2];
    p.orient = static_cast<Orientation>(ev.ints[3]);
    p.level = ev.ints[4];
    p.team = ev.text;
    _players[p.id] = std::move(p);
}

void BoardData::on_ppo(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 4)
        return;
    auto it = _players.find(ev.ints[0]);
    if (it == _players.end())
        return;
    it->second.x = ev.ints[1];
    it->second.y = ev.ints[2];
    it->second.orient = static_cast<Orientation>(ev.ints[3]);
}

void BoardData::on_plv(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 2)
        return;
    auto it = _players.find(ev.ints[0]);
    if (it != _players.end())
        it->second.level = ev.ints[1];
}

void BoardData::on_pin(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 9)
        return;
    auto it = _players.find(ev.ints[0]);
    if (it == _players.end())
        return;
    it->second.x = ev.ints[1];
    it->second.y = ev.ints[2];
    for (std::size_t i = 0; i < RESOURCE_COUNT; ++i)
        it->second.inventory[i] = ev.ints[3 + i];
}

void BoardData::on_pdi(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    auto it = _players.find(ev.ints[0]);
    if (it != _players.end())
        it->second.alive = false;
}

void BoardData::on_enw(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 4)
        return;
    EggData e;
    e.id = ev.ints[0];
    e.player_id = ev.ints[1];
    e.x = ev.ints[2];
    e.y = ev.ints[3];
    _eggs[e.id] = e;
}

void BoardData::on_edi(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    auto it = _eggs.find(ev.ints[0]);
    if (it != _eggs.end())
        it->second.alive = false;
}

void BoardData::on_seg(const net::ServerEvent &ev)
{
    _ended = true;
    _winner = ev.text;
}

void BoardData::on_pbc(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    push_player_event(GraphicalEventKind::BROADCAST, ev.ints[0], ev.text);
}

void BoardData::on_pic(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 3)
        return;
    GraphicalEvent gev;
    gev.kind = GraphicalEventKind::INCANTATION_START;
    gev.x = ev.ints[0];
    gev.y = ev.ints[1];
    gev.level = ev.ints[2];
    _events.push_back(std::move(gev));
}

void BoardData::on_pie(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 3)
        return;
    GraphicalEvent gev;
    gev.kind = GraphicalEventKind::INCANTATION_END;
    gev.x = ev.ints[0];
    gev.y = ev.ints[1];
    gev.success = ev.ints[2] != 0;
    _events.push_back(std::move(gev));
}

void BoardData::on_pex(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    push_player_event(GraphicalEventKind::EJECT, ev.ints[0], {});
}

void BoardData::on_pfk(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    push_player_event(GraphicalEventKind::FORK, ev.ints[0], {});
}

} // namespace zappy::gui::board_data
