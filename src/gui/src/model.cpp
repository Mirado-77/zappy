/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** model
*/

#include "model.hpp"

model::model(std::string name)
    : _anim(nullptr) ,_animeCount(0)
    , _current_frame(0), _anim_index(0),
    _color(WHITE), _name(name), _pos({0.0f, 0.0f, 0.0f})
{
    this->_model = LoadModel(name.c_str());
}

model::~model()
{
    UnloadModel(this->_model);
}

void model::Update_anim()
{
    this->_current_frame = (this->_current_frame + 1) % this->_anim.get()[this->_anim_index].keyframeCount;
    UpdateModelAnimation(this->_model, this->_anim.get()[this->_anim_index], (float)this->_current_frame);
}

void model::switch_index_anime()
{
    this->_anim_index = (this->_anim_index + 1) % this->_animeCount;
}

void model::Draw_model(float scale)
{
    DrawModel(this->_model, this->_pos, scale, this->_color);
}

void model::change_color(Color cl)
{
    this->_color = cl;
}

void model::load_anim()
{
    this->_anim.reset(LoadModelAnimations(_name.c_str(), &this->_animeCount));
}

void model::move_model(Vector3 pos, Vector3 rotationAxis, float rotationAngle, Vector3 scale)
{
    DrawModelEx(this->_model, pos, rotationAxis, rotationAngle, scale, this->_color);
}

void model::move_pos(Vector3 pos)
{
    this->_pos = pos;
}