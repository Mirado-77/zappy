##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## CRITICAL_SURVIVAL state handler
##

from fsm_helpers import find_food_tile, request_look
from look import tile_index_to_offset
from navigation import navigate_to
from player_state import PlayerState, apply_forward
from sender import CommandSender


def tick_critical_survival(state: PlayerState, sender: CommandSender) -> None:
    """Execute one FSM tick in the CRITICAL_SURVIVAL state.

    Priority actions:
    1. Food on current tile: take it immediately.
    2. Food visible elsewhere in Look result: navigate there.
    3. No Look result available: request one.
    4. Nothing visible: move forward one step and request a new Look.
    """
    if not sender.can_send:
        return

    # 1. Food on current tile.
    if state.last_look and "food" in state.last_look[0]:
        sender.send("Take food", callback=lambda r: None)
        return

    # 2. Food visible elsewhere.
    if state.last_look:
        food_idx = find_food_tile(state)
        if food_idx is not None and food_idx > 0:
            dx, dy = tile_index_to_offset(food_idx, state.level)
            tx = (state.x + dx) % state.map_width
            ty = (state.y + dy) % state.map_height
            navigate_to(state, sender, tx, ty)
            return

    # 3. No look result yet.
    if not state.last_look:
        request_look(state, sender)
        return

    # 4. Nothing visible: advance and refresh.
    sender.send("Forward",
                callback=lambda r: apply_forward(state) if r == "ok" else None)
    request_look(state, sender)
