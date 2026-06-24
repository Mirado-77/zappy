/*
** EPITECH PROJECT, 2026
** Renderer.cpp
** File description:
** Raylib window owner driven by BoardData
*/

#include "gui/render/Renderer.hpp"

#include <string>

#include "gui/draw.hpp"

namespace zappy::gui::render {

namespace {

constexpr int WIN_WIDTH = 1280;
constexpr int WIN_HEIGHT = 720;

const char *dir_char(board_data::Orientation dir)
{
    switch (dir) {
        case board_data::Orientation::NORTH: return "N";
        case board_data::Orientation::EAST: return "E";
        case board_data::Orientation::SOUTH: return "S";
        default: return "W";
    }
}

int count_flooded(const board_data::BoardData &board)
{
    int count = 0;

    for (const auto &tile : board.tiles())
        if (tile.flooded)
            ++count;
    return count;
}

} // namespace

Renderer::Renderer()
    : _window(WIN_WIDTH, WIN_HEIGHT, "Zappy GUI"),
      _camera({10.0f, 10.0f, 10.0f})
{
    _camera.set_mode(CAMERA_FREE);
    SetTargetFPS(60);
    _window.Set_window_state(FLAG_WINDOW_RESIZABLE);
}

bool Renderer::should_close()
{
    return _window.WindowIsCLose();
}

void Renderer::overlay(const board_data::BoardData &board)
{
    std::string line1 = "map " + std::to_string(board.width()) + "x"
        + std::to_string(board.height()) + "  tu "
        + std::to_string(board.time_unit());
    std::string line2 = "players " + std::to_string(board.players().size())
        + "  teams " + std::to_string(board.teams().size());

    ::draw::Draw_text(line1, {10, 10}, 20, DARKGRAY);
    ::draw::Draw_text(line2, {10, 35}, 20, DARKGRAY);
    if (board.game_ended())
        ::draw::Draw_text("WINNER: " + board.winner(), {10, 60}, 20, MAROON);
    if (_scene.active_meteors() > 0)
        ::draw::Draw_text("METEOR x"
            + std::to_string(_scene.active_meteors()), {10, 110}, 20, RED);
    const int floods = count_flooded(board);
    if (floods > 0)
        ::draw::Draw_text("FLOOD tiles=" + std::to_string(floods),
            {10, 135}, 20, BLUE);
}

void Renderer::storm_overlay(const board_data::BoardData &board)
{
    if (!board.storm().has_value())
        return;
    const auto &s = *board.storm();
    const std::string line = "STORM center=(" + std::to_string(s.center_x)
        + "," + std::to_string(s.center_y) + ")  R=" + std::to_string(s.radius)
        + "  dir=" + dir_char(s.direction);

    ::draw::Draw_text(line, {10, 85}, 20, ORANGE);
}

void Renderer::update_cursor()
{
    const bool look = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    if (look && !IsCursorHidden())
        DisableCursor();
    if (!look && IsCursorHidden())
        EnableCursor();
}

void Renderer::handle_pick(board_data::BoardData &board, ui::AdminPanel &admin,
    ui::PlayerPanel &player)
{
    const Vector2 mouse = GetMousePosition();

    if (!board.is_admin() || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)
        || !IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
        || admin.blocks_point(mouse) || player.blocks_point(mouse))
        return;
    const auto id = _scene.pick_player(mouse, _camera.cam);
    if (id)
        board.set_selected_player(*id);
}

void Renderer::draw_crosshair()
{
    const int cx = GetScreenWidth() / 2;
    const int cy = GetScreenHeight() / 2;

    DrawLine(cx - 8, cy, cx + 8, cy, DARKGRAY);
    DrawLine(cx, cy - 8, cx, cy + 8, DARKGRAY);
}

void Renderer::draw(board_data::BoardData &board, ui::AdminPanel &admin,
    ui::PlayerPanel &player)
{
    _scene.sync(board);
    update_cursor();
    _camera.update_free_rmb(!admin.capturing_keyboard());
    handle_pick(board, admin, player);
    if (IsKeyPressed(KEY_B) && !admin.capturing_keyboard())
        _scene.set_show_biomes(!_scene.show_biomes());
    if (IsKeyPressed(KEY_T) && !admin.capturing_keyboard())
        _scene.set_show_storm(!_scene.show_storm());
    if (IsKeyPressed(KEY_F) && !admin.capturing_keyboard())
        _scene.set_show_flood(!_scene.show_flood());
    ::draw::Start_Draw();
    ClearBackground(RAYWHITE);
    _camera.Begin_mode_3d();
    _scene.draw(board);
    _camera.End_mode_3d();
    overlay(board);
    storm_overlay(board);
    if (IsCursorHidden())
        draw_crosshair();
    admin.draw();
    player.draw();
    ::draw::End_Draw();
}

} // namespace zappy::gui::render
