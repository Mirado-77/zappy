/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene renderer driven by GameState
*/

#ifndef GUI_RENDER_SCENE_HPP_
    #define GUI_RENDER_SCENE_HPP_

    #include <memory>
    #include <unordered_map>

    #include "gui/state/GameState.hpp"
    #include "gui/world.hpp"
    #include "gui/trantor.hpp"

namespace zappy::gui::render {

class Scene {
public:
    void sync(const state::GameState &state);
    void draw();

private:
    struct Slot {
        std::unique_ptr<::trantor> robot;
        int orientation = 1;
        bool dead = false;
    };

    void ensure_world(const state::GameState &state);
    void sync_player(int id, const state::Player &player);
    void rotate_to(Slot &slot, int target);

    std::unique_ptr<::world> _world;
    std::unordered_map<int, Slot> _slots;
    int _width = 0;
    int _height = 0;
};

} // namespace zappy::gui::render

#endif /* !GUI_RENDER_SCENE_HPP_ */
