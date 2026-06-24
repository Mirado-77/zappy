/*
** EPITECH PROJECT, 2026
** TileData.hpp
** File description:
** TileData
*/

#ifndef TILEDATA_HPP_
    #define TILEDATA_HPP_
    #include <array>
    #include <cstddef>

namespace zappy::gui::board_data {

constexpr std::size_t RESOURCE_COUNT = 7;

using Resources = std::array<int, RESOURCE_COUNT>;

enum class Orientation { NORTH = 1, EAST = 2, SOUTH = 3, WEST = 4 };

struct TileData {
    int x = 0;
    int y = 0;
    Resources res{};
};

} // namespace zappy::gui::board_data

#endif /* TILEDATA_HPP_ */
