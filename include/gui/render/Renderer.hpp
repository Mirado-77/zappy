/*
** EPITECH PROJECT, 2026
** Renderer.hpp
** File description:
** Raylib window owner driven by BoardData
*/

#ifndef GUI_RENDER_RENDERER_HPP_
    #define GUI_RENDER_RENDERER_HPP_

    #include "gui/board_data/BoardData.hpp"
    #include "gui/camera.hpp"
    #include "gui/render/Scene.hpp"
    #include "gui/ui/AdminPanel.hpp"
    #include "gui/window.hpp"

namespace zappy::gui::render {

class Renderer {
public:
    Renderer();

    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    [[nodiscard]] bool should_close();
    void draw(const board_data::BoardData &board, ui::AdminPanel &admin);

private:
    void overlay(const board_data::BoardData &board);

    window _window;
    camera _camera;
    Scene _scene;
};

} // namespace zappy::gui::render

#endif /* GUI_RENDER_RENDERER_HPP_ */
