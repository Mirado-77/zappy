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
    #define TILE_PATH "src/gui/assets/farm_grass-tile.glb"

class world {
    public:
        world(int x, int y);
        ~world();
        void draw_world();
    protected:
    private:
        std::vector<std::unique_ptr<model>> init_model(float y, float x);
        void model_animation();
        std::vector<std::vector<std::unique_ptr<model>>> _map;

};


#endif /* !MAP_HPP_ */
