##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## FORAGING state handler
##

from fsm_helpers import find_food_tile, request_look
from look import tile_index_to_offset
from navigation import navigate_to
from player_state import PlayerState, apply_forward
from sender import CommandSender


def tick_foraging(state: PlayerState, sender: CommandSender) -> None:
    """Execute one FSM tick in the FORAGING state.

    Opportunistically collects food when below the safe threshold.
    Yields immediately to CRITICAL_SURVIVAL if food drops further.

    Priority actions:
    1. Food on current tile: take it.
    2. Food visible in Look result: navigate there.
    3. Nothing visible: advance one step and request a new Look.
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

    # 3. Nothing visible: move forward and refresh Look.
    sender.send("Forward",
                callback=lambda r: apply_forward(state) if r == "ok" else None)
    request_look(state, sender)
