/*
** EPITECH PROJECT, 2026
** Zappy
** File description:
** Decoded line emitted by the Zappy server to a GUI client
*/

#ifndef GUI_NET_SERVEREVENT_HPP_
    #define GUI_NET_SERVEREVENT_HPP_

    #include <string>
    #include <string_view>
    #include <vector>

namespace zappy::gui::net {

enum class EventKind {
    MSZ, SGT, SST, TNA, BCT, PNW, PPO, PLV, PIN, PEX, PBC, PIC, PIE, PFK, PDR,
    PGT, PDI, ENW, EBO, EDI, SEG, SMG, SUC, SBP,
    OK, KO, ADM_FLAG_LIST, PPF, EVT_BIOME_SET,
    EVT_STORM_START, EVT_STORM_TICK, EVT_STORM_END,
    UNKNOWN
};

struct ServerEvent {
    EventKind kind = EventKind::UNKNOWN;
    std::vector<int> ints;
    std::string text;
};

class ServerEventParser {
public:
    [[nodiscard]] static ServerEvent parse(std::string_view line);
};

} // namespace zappy::gui::net

#endif /* !GUI_NET_SERVEREVENT_HPP_ */
