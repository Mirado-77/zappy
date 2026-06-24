/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Scene renderer driven by BoardData
*/

#ifndef GUI_RENDER_SCENE_HPP_
    #define GUI_RENDER_SCENE_HPP_

    #include <raylib.h>

    #include <cstddef>
    #include <memory>
    #include <optional>
    #include <unordered_map>
    #include <vector>

    #include "gui/board_data/BoardData.hpp"
    #include "gui/world.hpp"
    #include "gui/trantor.hpp"

namespace zappy::gui::render {

class Scene {
public:
    void sync(board_data::BoardData &board);
    void draw(const board_data::BoardData &board);
    [[nodiscard]] std::size_t active_meteors() const noexcept
        { return _meteors.size(); }
    [[nodiscard]] std::optional<int> pick_player(Vector2 screen,
        const Camera &cam) const;
    [[nodiscard]] bool show_biomes() const noexcept { return _show_biomes; }
    void set_show_biomes(bool value) noexcept { _show_biomes = value; }
    [[nodiscard]] bool show_storm() const noexcept { return _show_storm; }
    void set_show_storm(bool value) noexcept { _show_storm = value; }
    [[nodiscard]] bool show_flood() const noexcept { return _show_flood; }
    void set_show_flood(bool value) noexcept { _show_flood = value; }

private:
    struct Slot {
        std::unique_ptr<::trantor> robot;
        int orientation = 1;
        bool dead = false;
        int x = 0;
        int y = 0;
    };

    struct MeteorView {
        int x = 0;
        int y = 0;
        int radius = 0;
        double start_time = 0.0;
    };

    void ensure_world(const board_data::BoardData &board);
    void sync_player(int id, const board_data::PlayerData &player);
    void rotate_to(Slot &slot, int target);
    void apply_biome_models(const board_data::BoardData &board);
    void draw_storm(const board_data::StormZone &storm);
    void draw_storm_arrow(Vector3 center, board_data::Orientation dir);
    void draw_meteor(const MeteorView &view);
    void draw_meteors();
    void draw_flood_overlays(const board_data::BoardData &board);

    std::unique_ptr<::world> _world;
    std::unordered_map<int, Slot> _slots;
    std::vector<MeteorView> _meteors;
    int _width = 0;
    int _height = 0;
    bool _show_biomes = true;
    bool _show_storm = true;
    bool _show_flood = true;
};

} // namespace zappy::gui::render

#endif /* !GUI_RENDER_SCENE_HPP_ */
