/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** trantor
*/

#include "gui/trantor.hpp"
#include "gui/utils.hpp"
#include "gui/Constants.hpp"
#include "gui/Item.hpp"

/*
    // int id = 0;
    // int x = 0;
    // int y = 0;
    // Orientation orient = Orientation::NORTH;
    int level = 1;
    std::string team;
    Resources inventory{};
    bool alive = true;
    _id(player_data.id),
    _pos(player_pos),
    _state(YES),
    _angle((player_data.orient - 1) * 90)
*/

inline double health_function(int food_count)
{
    return 100.0f * food_count / (food_count + SATISFYING_FOOD_COUNT / 9.0f);
}

trantor::trantor(const zappy::gui::board_data::PlayerData& trantorian_data, Vector3 player_pos)
    : _id(trantorian_data.id),
    _pos(player_pos),
    _state(YES),
    _angle((static_cast<int>(trantorian_data.orient) - 1) * 90),
    _health(health_function(trantorian_data.inventory[R_FOOD])),
    _is_alive(trantorian_data.alive),
    _death_frame(0)
{
    if (!_is_alive) {
        this->_state = DIE;
        _death_frame = TRANTORIAN_DEATH_FRAME;
    }
    if (!_mod->isloaded()) {
        _mod->load();
        this->_mod->load_anim();
    }
};

void trantor::Turn_left()
{
    this->_angle += 90;
};

void trantor::Turn_right()
{
    this->_angle -= 90;
};

void trantor::look_around()
{
    this->_state = LOOK_ARROUND;
};

void trantor::eject_player()
{
    this->_state = PUNCH;
};

void trantor::death()
{
    _health_bar_batch->kill(_id);
    this->_state = DIE;
};

void trantor::sync(const zappy::gui::board_data::PlayerData& trantorian_data, Vector3 player_pos)
{
    _id = trantorian_data.id;
    _pos = player_pos;
    _state = YES;
    _angle = (static_cast<int>(trantorian_data.orient) - 1) * 90;
    _health = health_function(trantorian_data.inventory[R_FOOD]);
    _is_alive = trantorian_data.alive;
    if (_is_alive)
        _health_bar_batch->sync_health_bar(_id, _pos, _health);
}

void trantor::set_pos(Vector3 pos)
{
    _pos = pos;
};

void trantor::set_shader(Shader shader)
{
    this->_mod->set_shader(shader);
};

void trantor::Update_anim()
{
    this->_mod->Update_anim();
};

void trantor::draw()
{
    this->_mod->set_pos(_pos);
    this->_mod->set_anim_index(_state);
    this->_mod->set_angle(this->_angle);
    if (_state == DIE) {
        if (_death_frame < TRANTORIAN_DEATH_FRAME)
            ++_death_frame;
        this->_mod->Draw_model(_death_frame);
    } else
        this->_mod->Draw_model();
};
