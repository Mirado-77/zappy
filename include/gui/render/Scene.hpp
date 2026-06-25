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
    #include <utility>
    #include <vector>

    #include "gui/board_data/BoardData.hpp"
    #include "gui/world.hpp"
    #include "gui/trantor.hpp"
    #include "gui/Egg.hpp"
    #include "gui/Item.hpp"
    #include "gui/board_data/GraphicalEvent.hpp"
    #include "gui/Incantation.hpp"

namespace zappy::gui::render {

struct PairHash {
    std::size_t operator()(
        const std::pair<std::size_t, std::size_t>& p
    ) const noexcept
    {
        return std::hash<std::size_t>{}(p.first)
            ^ (std::hash<std::size_t>{}(p.second) << 1);
    }
};

class Scene {
public:
    void sync(board_data::BoardData &board);
    void draw(const board_data::BoardData &board);
    void set_shader(Shader shader);
    [[nodiscard]] std::optional<int> pick_player(Vector2 screen,
        const Camera &cam) const;
    [[nodiscard]] std::optional<std::pair<int, int>> pick_tile(Vector2 screen,
        const Camera &cam) const;
    [[nodiscard]] std::size_t active_meteors() const noexcept
        { return _meteors.size(); }
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

    struct EggSlot {
        std::unique_ptr<::Egg> egg;
        bool dead = false;
    };

    struct Incant {
        std::unique_ptr<::Incantation> incant;
        bool dead = false;
    };

    struct MeteorView {
        int x = 0;
        int y = 0;
        int radius = 0;
        double start_time = 0.0;
    };

    [[nodiscard]] Vector3 world_pos(int x, int y) const;
    [[nodiscard]] BoundingBox player_box(int x, int y)const;
    void ensure_world(const board_data::BoardData &board);
    void sync_player(int id, const board_data::PlayerData &player);
    void sync_egg(int id, const board_data::EggData &egg);
    void sync_item(const board_data::TileData &tile,
        std::pair<std::size_t, std::size_t> coords);
    void sync_items(const board_data::BoardData &board);
    void drain_meteors(board_data::BoardData &board);
    void rotate_to(Slot &slot, int target);
    void sync_incant(int id, const board_data::GraphicalEvent &event);
    void apply_biome_models(const board_data::BoardData &board);
    [[nodiscard]] int tile_model_index(const board_data::TileData &tile) const;
    void draw_storm(const board_data::StormZone &storm);
    void draw_storm_arrow(Vector3 center, board_data::Orientation dir);
    void draw_meteor(const MeteorView &view);
    void draw_meteors();
    void draw_egg_player();

    std::unique_ptr<::world> _world;
    std::unordered_map<int, Slot> _slots;
    std::unordered_map<int, EggSlot> _eggs;
    std::unordered_map<int, Incant> _incantation;
    std::unordered_map<std::pair<std::size_t, std::size_t>,
        std::unique_ptr<::Item>, PairHash> _items;
    std::vector<MeteorView> _meteors;
    Shader _shader{};
    bool _has_shader = false;
    int _width = 0;
    int _height = 0;
    bool _show_biomes = true;
    bool _show_storm = true;
    bool _show_flood = true;
};

} // namespace zappy::gui::render

#endif /* !GUI_RENDER_SCENE_HPP_ */
