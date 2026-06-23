/*
** EPITECH PROJECT, 2026
** zappy_raylib
** File description:
** camera
*/

#include "gui/camera.hpp"
#include <iostream>
#include <map>


camera::camera(Vector3 pos)
    :  cameraMode(CAMERA_FREE)
{
    this->cam.position = pos;
    this->cam.fovy = 45.0f;
    this->cam.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    this->cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    this->cam.projection = CAMERA_PERSPECTIVE;
}

void camera::set_mode(CameraMode mode)
{
    cameraMode = mode;
    this->cam.up = (Vector3){ 0.0f, 1.0f, 0.0f };
}

void camera::update_camera()
{
    UpdateCamera(&this->cam, cameraMode);
}

void camera::set_pos(Vector3 pos)
{
    this->cam.position = pos;
}

void camera::set_target(Vector3 pos)
{
    this->cam.target = pos;
}

void camera::set_up(Vector3 pos)
{
    this->cam.up = pos;
}

void camera::set_fovy(float pos)
{
    this->cam.fovy = pos;
}

void camera::Begin_mode_3d()
{
    BeginMode3D(this->cam);
}

void camera::End_mode_3d()
{
    EndMode3D();
}
