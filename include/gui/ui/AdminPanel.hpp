/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** raygui admin authentication and feature-flag panel
*/

#ifndef GUI_UI_ADMINPANEL_HPP_
    #define GUI_UI_ADMINPANEL_HPP_

    #include <string>

    #include "gui/board_data/BoardData.hpp"
    #include "gui/net/ServerLink.hpp"

namespace zappy::gui::ui {

class AdminPanel {
public:
    AdminPanel(net::ServerLink &link, board_data::BoardData &board);

    void draw();

private:
    void handle_toggle();
    void sync_status();
    void draw_window();
    void draw_login();
    void draw_flags();
    void draw_flag_row(const std::string &name, bool on, float y);
    void draw_status();
    void submit_login();

    net::ServerLink &_link;
    board_data::BoardData &_board;
    bool _visible = false;
    bool _password_edit = false;
    char _password[64] = {0};
    unsigned _last_seq = 0;
    double _status_until = 0.0;
    std::string _status_text;
};

} // namespace zappy::gui::ui

#endif /* !GUI_UI_ADMINPANEL_HPP_ */
