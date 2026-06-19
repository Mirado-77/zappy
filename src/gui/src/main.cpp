/*
** EPITECH PROJECT, 2026
** $
** File description:
** main
*/

#include "draw.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "world.hpp"

int main()
{
    window wind(800, 450, "MY big BAZOUZOUZ");
    camera cam({10.0f, 10.0f, 10.0f});
    //model mod("src/gui/assets/farm_grass-tile.glb");
    world map(4, 8);
    cam.set_mode(CAMERA_FREE);
    DisableCursor();
    SetTargetFPS(60);
    wind.Set_window_state(FLAG_WINDOW_RESIZABLE);
    Vector3 Position = { 0.0f, 0.0f, 0.0f};
    while (!wind.WindowIsCLose()) {
        cam.update_camera();
        //if (IsKeyPressed(KEY_RIGHT))
        //    mod.switch_index_anime();
        //mod.Update_anim();
        draw::Start_Draw();
        ClearBackground(RAYWHITE);

        cam.Begin_mode_3d();
            map.draw_world();
            //DrawGrid(10, 1.0f);

        cam.End_mode_3d();

        draw::Draw_text("Welcome to the third dimension!", {10, 40}, 20, DARKGRAY);
        draw::Draw_text("Use the LEFT/RIGHT keys to switch animation", {10, 10}, 20, GRAY);

        draw::End_Draw();
    }
    return 0;
}