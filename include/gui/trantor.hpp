/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** trantor
*/

#ifndef TRANTOR_HPP_
    #define TRANTOR_HPP_
    #include "model.hpp"
    #define TRANTOR_PATH "src/gui/assets/robot.glb"

using TrantorState = enum TRANTOR_STATE {
    DANCE,
    DIE,
    LOOK_ARROUND,
    JUMP,
    HEAD_NO,
    PUNCH,
    RUN,
    SIT,
    STAND_UP,
    HAND_SHAKE,
    WALK,
    WALK_JUMP,
    WAVE,
    YES,
};

class trantor {
    public:
        trantor();
        ~trantor() = default;
        void Turn_left();
        void Turn_right();
        void look_around();
        void eject_player();
        void death();
        void set_pos(Vector3 pos);
        void set_shader(Shader shader);
        void draw();
        static std::unique_ptr<model> _mod;
    protected:
    private:
        Vector3 _pos;
        TrantorState _state;
        int _angle;
};

#endif /* !TRANTOR_HPP_ */
