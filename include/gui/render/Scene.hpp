/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene renderer driven by BoardData
*/

#ifndef GUI_RENDER_SCENE_HPP_
    #define GUI_RENDER_SCENE_HPP_

    #include <raylib.h>

    #include <memory>
    #include <optional>
    #include <unordered_map>

    #include "gui/board_data/BoardData.hpp"
    #include "gui/world.hpp"
    #include "gui/trantor.hpp"

namespace zappy::gui::render {

class Scene {
public:
    void sync(const board_data::BoardData &board);
    void draw();
    [[nodiscard]] std::optional<int> pick_player(Vector2 screen,
        const Camera &cam) const;
    [[nodiscard]] bool show_biomes() const noexcept { return _show_biomes; }
    void set_show_biomes(bool value) noexcept { _show_biomes = value; }

private:
    struct Slot {
        std::unique_ptr<::trantor> robot;
        int orientation = 1;
        bool dead = false;
        int x = 0;
        int y = 0;
    };

    void ensure_world(const board_data::BoardData &board);
    void sync_player(int id, const board_data::PlayerData &player);
    void rotate_to(Slot &slot, int target);
    void apply_biome_models(const board_data::BoardData &board);

    std::unique_ptr<::world> _world;
    std::unordered_map<int, Slot> _slots;
    int _width = 0;
    int _height = 0;
    bool _show_biomes = true;
};

} // namespace zappy::gui::render

#endif /* !GUI_RENDER_SCENE_HPP_ */
