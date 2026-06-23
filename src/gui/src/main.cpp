/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** main
*/

#include <iostream>

#include "gui/cli/GuiCliParser.hpp"
#include "gui/net/ServerLink.hpp"
#include "gui/state/GameState.hpp"
#include "gui/draw.hpp"
#include "gui/window.hpp"
#include "gui/camera.hpp"
#include "gui/model.hpp"
#include "gui/world.hpp"
#include "gui/trantor.hpp"

namespace {

void pump_network(zappy::gui::net::ServerLink &link,
    zappy::gui::state::GameState &state)
{
    link.poll_once(0);
    while (auto ev = link.next_message())
        state.apply(*ev);
}

void handle_input(camera &cam, trantor &mod)
{
    cam.update_camera();
    if (IsKeyPressed(KEY_RIGHT))
        mod.Turn_left();
    if (IsKeyPressed(KEY_LEFT))
        mod.Turn_right();
    if (IsKeyPressed(KEY_UP))
        mod.death();
}

void render_frame(camera &cam, trantor &mod, world &map)
{
    draw::Start_Draw();
    ClearBackground(RAYWHITE);
    cam.Begin_mode_3d();
    mod.draw();
    map.draw_world();
    cam.End_mode_3d();
    draw::Draw_text("Welcome to the third dimension!",
        {10, 40}, 20, DARKGRAY);
    draw::Draw_text("Use the LEFT/RIGHT keys to switch animation",
        {10, 10}, 20, GRAY);
    draw::End_Draw();
    
}

void setup_window(window &wind, camera &cam)
{
    cam.set_mode(CAMERA_FREE);
    DisableCursor();
    SetTargetFPS(60);
    wind.Set_window_state(FLAG_WINDOW_RESIZABLE);
}

int run_gui(const zappy::gui::cli::GuiOptions &opts)
{
    zappy::gui::net::ServerLink link(opts.host, opts.port);
    zappy::gui::state::GameState state;
    window wind(800, 450, "MY big BAZOUZOUZ");
    camera cam({10.0f, 10.0f, 10.0f});
    trantor mod;
    world map(4, 8);

    setup_window(wind, cam);
    while (!wind.WindowIsCLose()) {
        pump_network(link, state);
        handle_input(cam, mod);
        render_frame(cam, mod, map);
    }
    return 0;
}

int parse_cli(int argc, char **argv, zappy::gui::cli::GuiOptions &out)
{
    try {
        out = zappy::gui::cli::GuiCliParser::parse(argc, argv);
    } catch (const zappy::gui::cli::GuiCliError &e) {
        std::cerr << e.what() << '\n';
        zappy::gui::cli::GuiCliParser::print_usage(std::cerr, argv[0]);
        return 84;
    }
    return 0;
}

} // namespace

int main(int argc, char **argv)
{
    zappy::gui::cli::GuiOptions opts;
    if (int rc = parse_cli(argc, argv, opts); rc != 0)
        return rc;
    try {
        return run_gui(opts);
    } catch (const zappy::gui::net::ServerLinkError &e) {
        std::cerr << "[NETWORK ERROR] " << e.what() << '\n';
        return 84;
    }
}