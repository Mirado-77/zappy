/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** main
*/

#include <iostream>

#include "gui/cli/GuiCliParser.hpp"
#include "gui/net/ServerLink.hpp"
#include "gui/render/Scene.hpp"
#include "gui/state/GameState.hpp"
#include "gui/draw.hpp"
#include "gui/window.hpp"
#include "gui/camera.hpp"

namespace {

void pump_network(zappy::gui::net::ServerLink &link,
    zappy::gui::state::GameState &state)
{
    link.poll_once(0);
    while (auto ev = link.next_message())
        state.apply(*ev);
}

void render_frame(camera &cam, zappy::gui::render::Scene &scene)
{
    draw::Start_Draw();
    ClearBackground(RAYWHITE);
    cam.Begin_mode_3d();
    scene.draw();
    cam.End_mode_3d();
    draw::Draw_text("Zappy GUI", {10, 10}, 20, DARKGRAY);
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
    window wind(800, 450, "Zappy");
    camera cam({10.0f, 10.0f, 10.0f});
    zappy::gui::net::ServerLink link(opts.host, opts.port);
    zappy::gui::state::GameState state;
    zappy::gui::render::Scene scene;

    setup_window(wind, cam);
    while (!wind.WindowIsCLose()) {
        pump_network(link, state);
        scene.sync(state);
        cam.update_camera();
        render_frame(cam, scene);
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
