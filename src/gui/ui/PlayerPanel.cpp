/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** raygui player profile panel implementation
*/

#include "gui/ui/PlayerPanel.hpp"

#include <array>
#include <cstddef>

#include "raygui.h"

namespace zappy::gui::ui {

namespace {

constexpr float PANEL_W = 280.0f;
constexpr float PANEL_H = 320.0f;

constexpr std::array<const char *, board_data::RESOURCE_COUNT> RES_NAMES = {
    "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"
};

const char *orient_str(board_data::Orientation o)
{
    switch (o) {
        case board_data::Orientation::NORTH: return "N";
        case board_data::Orientation::EAST: return "E";
        case board_data::Orientation::SOUTH: return "S";
        default: return "W";
    }
}

} // namespace

PlayerPanel::PlayerPanel(board_data::BoardData &board)
    : _board(board)
{
}

Rectangle PlayerPanel::panel_rect() const
{
    return {static_cast<float>(GetScreenWidth()) - PANEL_W - 20.0f,
        80.0f, PANEL_W, PANEL_H};
}

bool PlayerPanel::blocks_point(Vector2 p) const
{
    return _board.selected_player_id() >= 0
        && CheckCollisionPointRec(p, panel_rect());
}

void PlayerPanel::draw()
{
    const int id = _board.selected_player_id();

    if (id < 0)
        return;
    const Rectangle box = panel_rect();
    if (GuiWindowBox(box, "Player")) {
        _board.clear_selection();
        return;
    }
    if (_board.profile_disabled())
        draw_disabled(box);
    else
        draw_profile(box, id);
}

void PlayerPanel::draw_header(int x, int y, int id,
    const board_data::PlayerData &p)
{
    DrawText(TextFormat("id: #%d", id), x, y, 20, DARKGRAY);
    DrawText(TextFormat("team: %s", p.team.c_str()), x, y + 20, 20, DARKGRAY);
    DrawText(TextFormat("pos: %d,%d", p.x, p.y), x, y + 40, 20, DARKGRAY);
    DrawText(TextFormat("orient: %s", orient_str(p.orient)),
        x, y + 60, 20, DARKGRAY);
    DrawText(TextFormat("level: %d", p.level), x, y + 80, 20, DARKGRAY);
}

void PlayerPanel::draw_inventory(int x, int y,
    const board_data::PlayerData &p)
{
    DrawText("inventory:", x, y, 20, DARKGRAY);
    for (std::size_t i = 0; i < board_data::RESOURCE_COUNT; ++i)
        DrawText(TextFormat("%s: %d", RES_NAMES[i], p.inventory[i]),
            x, y + 20 + static_cast<int>(i) * 16, 20, DARKGRAY);
}

void PlayerPanel::draw_buttons(const Rectangle &box, int id)
{
    const float by = box.y + box.height - 32;

    if (GuiButton({box.x + 12, by, 110, 24}, "Refresh"))
        _board.set_selected_player(id);
    if (GuiButton({box.x + box.width - 122, by, 110, 24}, "Close"))
        _board.clear_selection();
}

void PlayerPanel::draw_profile(const Rectangle &box, int id)
{
    auto it = _board.players().find(id);

    if (it == _board.players().end())
        return;
    const int x = static_cast<int>(box.x) + 12;
    const int y = static_cast<int>(box.y) + 30;
    draw_header(x, y, id, it->second);
    draw_inventory(x, y + 105, it->second);
    draw_buttons(box, id);
}

void PlayerPanel::draw_disabled(const Rectangle &box)
{
    const int x = static_cast<int>(box.x) + 12;
    const int y = static_cast<int>(box.y) + 40;

    DrawText("profile flag", x, y, 18, MAROON);
    DrawText("disabled on server", x, y + 22, 18, MAROON);
    if (GuiButton({box.x + 12, box.y + box.height - 32, 110, 24}, "Close"))
        _board.clear_selection();
}

} // namespace zappy::gui::ui
