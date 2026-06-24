/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** Egg
*/

#ifndef EGG_HPP_
    #define EGG_HPP_
    #include "model.hpp"
    #define EGG_PATH "src/gui/assets/egg.glb"

class Egg {
    public:
        Egg();
        ~Egg() = default;
        void draw();
        void set_pos(Vector3);
    protected:
    private:
        model _mod;
};

#endif /* !EGG_HPP_ */
