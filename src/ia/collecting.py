##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## COLLECTING state handler
##

from fsm_helpers import request_look
from look import tile_index_to_offset
from navigation import navigate_to
from player_state import PlayerState, apply_forward, apply_right
from sender import CommandSender
from stones import needed_stones, STONE_NAMES

# Turn right every N forward steps when roaming to produce a loose spiral.
ROAM_TURN_INTERVAL = 4


def find_needed_stone_tile(
        state: PlayerState,
        wanted: dict[str, int]) -> tuple[int, str] | None:
    """Return (tile_index, stone_name) of the nearest needed stone, or None.

    Scans last_look in tile-index order. Only returns a stone that is
    present in the wanted dict (over-collection prevention).
    """
    for idx, tile in enumerate(state.last_look):
        for stone in STONE_NAMES:
            if stone in tile and stone in wanted:
                return idx, stone
    return None


def tick_collecting(state: PlayerState, sender: CommandSender,
                    roam_counter: list[int]) -> None:
    """Execute one FSM tick in the COLLECTING state.

    Priority actions:
    1. Needed stone on current tile: take it.
    2. Needed stone visible on another tile: navigate there.
    3. No Look result available: request one.
    4. Nothing useful visible: roam (forward + periodic right turn).

    The roam_counter list holds a single integer mutated in place
    across ticks.
    """
    if not sender.can_send:
        return

    wanted = needed_stones(state.level, state.inventory)
    if not wanted:
        return   # evaluator will transition state next tick

    # 1. Needed stone on own tile.
    if state.last_look:
        for stone in STONE_NAMES:
            if stone in state.last_look[0] and stone in wanted:
                sender.send(f"Take {stone}", callback=lambda r: None)
                return

    # 2. Needed stone visible elsewhere.
    if state.last_look:
        match = find_needed_stone_tile(state, wanted)
        if match is not None:
            idx, stone = match
            dx, dy = tile_index_to_offset(idx, state.level)
            tx = (state.x + dx) % state.map_width
            ty = (state.y + dy) % state.map_height
            navigate_to(state, sender, tx, ty)
            return

    # 3. No Look result: request one.
    if not state.last_look:
        request_look(state, sender)
        return

    # 4. Nothing useful visible: roam.
    roam_counter[0] += 1
    if roam_counter[0] % ROAM_TURN_INTERVAL == 0:
        sender.send("Right",
                    callback=lambda r: apply_right(state) if r == "ok" else None)
    else:
        sender.send("Forward",
                    callback=lambda r: apply_forward(state) if r == "ok" else None)
    state.last_look = []   # invalidate so a fresh Look is requested next tick
    request_look(state, sender)
