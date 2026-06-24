/*
** EPITECH PROJECT, 2026
** local-zappy
** File description:
** model
*/

#ifndef MODEL_HPP_
    #define MODEL_HPP_
    #include <iostream>
    #include <memory>
    #include <raylib.h>


class model{
    public:
        model(std::string name);
        ~model();
        void Update_anim();
        void load_anim();
        void load_texture();
        void load_texture(std::string name);
        void switch_index_anime();
        void Draw_model(float scale = 1.0f);
        void change_color(Color cl);
        void move_model(Vector3 rotationAxis, float rotationAngle, Vector3 scale);
        void set_pos(Vector3 pos);
        void set_base_pos(Vector3 pos);
        Vector3 get_pos();
        Vector3 get_base_pos();
        void set_anim_index(int anime_index);
        //void Load_model(std::string name);
        //void update_model_anim();
    protected:
    private:
        Model _model;
        std::unique_ptr<ModelAnimation> _anim;
        int _animeCount;
        int _current_frame;
        int _anim_index;
        Color _color;
        std::string _name;
        Vector3 _pos;
        Vector3 _base_pos;
        Texture2D _texture;
};

#endif /* !MODEL_HPP_ */
