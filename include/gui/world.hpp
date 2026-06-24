/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** world
*/

#ifndef MAP_HPP_
    #define MAP_HPP_
    #include "model.hpp"
    #include <raymath.h>
    #include <memory>
    #include <vector>

class world {
    public:
        world(int x, int y);
        ~world();
        void draw_world();
        void set_tile_model(int x, int y, int model_index);
    protected:
    private:
        void load_palette();
        void draw_tile(int x, int y, double time, float scale);
        std::vector<std::unique_ptr<model>> _palette;
        std::vector<std::vector<int>> _index;
        int _width;
        int _height;

};


#endif /* !MAP_HPP_ */
