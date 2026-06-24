/*
** EPITECH PROJECT, 2026
** Renderer.cpp
** File description:
** Raylib window owner driven by BoardData
*/

#include "gui/render/Renderer.hpp"
#include "gui/utils.hpp"

#include <string>

#include "gui/draw.hpp"

namespace zappy::gui::render {

namespace {

constexpr int WIN_WIDTH = 1280;
constexpr int WIN_HEIGHT = 720;

} // namespace

Renderer::Renderer()
    : _window(WIN_WIDTH, WIN_HEIGHT, "Zappy GUI"),
      _camera({10.0f, 10.0f, 10.0f})
{
    _camera.set_mode(CAMERA_FREE);
    SetTargetFPS(60);
    _window.Set_window_state(FLAG_WINDOW_RESIZABLE);
    DisableCursor();
}

bool Renderer::should_close()
{
    return _window.WindowIsCLose();
}

void Renderer::overlay(const board_data::BoardData &board)
{
    std::string hud_map_info = "map " + std::to_string(board.width()) + "x"
        + std::to_string(board.height()) + "  tu "
        + std::to_string(board.time_unit());
    std::string hud_players_info = "players " + std::to_string(board.players().size())
        + "  teams " + std::to_string(board.teams().size());

    ::draw::Draw_text(hud_map_info, {HUD_PADDING, HUD_PADDING}, 20, DARKGRAY);
    ::draw::Draw_text(hud_players_info, {HUD_PADDING, HUD_PADDING + HUD_LINE_SPACING}, 20, DARKGRAY);
    if (board.game_ended())
        ::draw::Draw_text("WINNER: " + board.winner(), {HUD_PADDING, HUD_CENTER}, 20, MAROON);
}

void Renderer::draw(const board_data::BoardData &board)
{
    _scene.sync(board);
    _camera.update_camera();
    ::draw::Start_Draw();
    ClearBackground(RAYWHITE);
    _camera.Begin_mode_3d();
    _scene.draw();
    _camera.End_mode_3d();
    overlay(board);
    ::draw::End_Draw();
}

} // namespace zappy::gui::render
