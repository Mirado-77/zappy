/*
** EPITECH PROJECT, 2026
** zappy_raylib
** File description:
** draw
*/

#ifndef RAYLIB_HPP_
    #define RAYLIB_HPP_
    #include <iostream>
    #include <raylib.h>
    #include <raymath.h>

class draw {
    public:
        static void End_Draw();
        static void Start_Draw();
        static void Draw_text(std::string text, Vector2 pos, int fontsize, Color color);
        static void Draw_fps(int posX, int posY);
        static void Draw_rec(Vector2 pos, int width, int height, Color color);
        static void Draw_rec_line(Vector2 pos, int width, int height, Color color);
        static void Draw_grid(int slice, float spacing);
    protected:
    private:

};

#endif /* !DRAW_HPP_ */
