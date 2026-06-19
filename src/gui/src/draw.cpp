/*
** EPITECH PROJECT, 2026
** zappy_raylib
** File description:
** draw
*/

#include "draw.hpp"

void draw::End_Draw()
{
    EndDrawing();
}

void draw::Start_Draw()
{
    BeginDrawing();

}

void draw::Draw_text(std::string text, Vector2 pos, int fontsize, Color color)
{
    DrawText(text.c_str(), pos.x, pos.y, fontsize, color);
}

void draw::Draw_fps(int posX, int posY)
{
    DrawFPS(posX, posY);
}

void draw::Draw_rec(Vector2 pos, int width, int height, Color color)
{
    DrawRectangle(pos.x, pos.y, width, height, color);
}

void draw::Draw_rec_line(Vector2 pos, int width, int height, Color color)
{
    DrawRectangleLines(pos.x, pos.y, width, height, color);
}

void draw::Draw_grid(int slice, float spacing)
{
    DrawGrid(slice, spacing);
}