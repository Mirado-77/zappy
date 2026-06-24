/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** raygui panel showing the extended profile of a selected player
*/

#ifndef GUI_UI_PLAYERPANEL_HPP_
    #define GUI_UI_PLAYERPANEL_HPP_

    #include <raylib.h>

    #include "gui/board_data/BoardData.hpp"

namespace zappy::gui::ui {

class PlayerPanel {
public:
    explicit PlayerPanel(board_data::BoardData &board);

    void draw();
    [[nodiscard]] bool blocks_point(Vector2 p) const;

private:
    [[nodiscard]] Rectangle panel_rect() const;
    void draw_profile(const Rectangle &box, int id);
    void draw_disabled(const Rectangle &box);
    void draw_header(int x, int y, int id, const board_data::PlayerData &p);
    void draw_inventory(int x, int y, const board_data::PlayerData &p);
    void draw_buttons(const Rectangle &box, int id);

    board_data::BoardData &_board;
};

} // namespace zappy::gui::ui

#endif /* !GUI_UI_PLAYERPANEL_HPP_ */
