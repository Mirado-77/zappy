/*
** EPITECH PROJECT, 2026
** BoardData.hpp
** File description:
** BoardData
*/

#ifndef BOARDDATA_HPP_
    #define BOARDDATA_HPP_
    #include "../ipc/ServerInfo.hpp"
    #include <vector>

namespace zappy::gui::board_data {

class BoardData {
public:
    BoardData(const ipc::ServerInfo &server_info);
    tileData &get_tile_value(size_t x, size_t y);
private:
    ipc::ServerInfo server_info;
    size_t width;
    size_t height;
    std::vector<TileData> tiles;
    std::vector<GraphicalEvent> events;
}
    
} // namespace zappy::gui::ipc

#endif /* BOARDDATA_HPP_ */
