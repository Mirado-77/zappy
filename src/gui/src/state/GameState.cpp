/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** GameState implementation
*/

#include "gui/state/GameState.hpp"

#include <stdexcept>

namespace zappy::gui::state {

void GameState::apply(const net::ServerEvent &ev)
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
        default: break;
    }
}

const Tile &GameState::tile(int x, int y) const
{
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        throw std::out_of_range("tile() out of bounds");
    return _tiles[idx(x, y)];
}

void GameState::resize_tiles()
{
    _tiles.assign(static_cast<std::size_t>(_width) * _height, Tile{});
    for (int y = 0; y < _height; ++y)
        for (int x = 0; x < _width; ++x)
            _tiles[idx(x, y)] = Tile{x, y, {}};
}

void GameState::on_msz(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 2)
        return;
    _width = ev.ints[0];
    _height = ev.ints[1];
    resize_tiles();
}

void GameState::on_sgt(const net::ServerEvent &ev)
{
    if (!ev.ints.empty())
        _time_unit = ev.ints[0];
}

void GameState::on_tna(const net::ServerEvent &ev)
{
    if (!ev.text.empty())
        _teams.push_back(ev.text);
}

void GameState::on_bct(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 9)
        return;
    const int x = ev.ints[0];
    const int y = ev.ints[1];
    if (x < 0 || y < 0 || x >= _width || y >= _height)
        return;
    Tile &t = _tiles[idx(x, y)];
    t.x = x;
    t.y = y;
    for (std::size_t i = 0; i < RESOURCE_COUNT; ++i)
        t.res[i] = ev.ints[2 + i];
}

void GameState::on_pnw(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 5)
        return;
    Player p;
    p.id = ev.ints[0];
    p.x = ev.ints[1];
    p.y = ev.ints[2];
    p.orient = static_cast<Orientation>(ev.ints[3]);
    p.level = ev.ints[4];
    p.team = ev.text;
    _players[p.id] = std::move(p);
}

void GameState::on_ppo(const net::ServerEvent &ev)
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

void GameState::on_plv(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 2)
        return;
    auto it = _players.find(ev.ints[0]);
    if (it != _players.end())
        it->second.level = ev.ints[1];
}

void GameState::on_pin(const net::ServerEvent &ev)
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

void GameState::on_pdi(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    auto it = _players.find(ev.ints[0]);
    if (it != _players.end())
        it->second.alive = false;
}

void GameState::on_enw(const net::ServerEvent &ev)
{
    if (ev.ints.size() < 4)
        return;
    Egg e;
    e.id = ev.ints[0];
    e.player_id = ev.ints[1];
    e.x = ev.ints[2];
    e.y = ev.ints[3];
    _eggs[e.id] = e;
}

void GameState::on_edi(const net::ServerEvent &ev)
{
    if (ev.ints.empty())
        return;
    auto it = _eggs.find(ev.ints[0]);
    if (it != _eggs.end())
        it->second.alive = false;
}

void GameState::on_seg(const net::ServerEvent &ev)
{
    _ended = true;
    _winner = ev.text;
}

} // namespace zappy::gui::state