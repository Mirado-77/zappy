##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## Shared FSM helpers: inventory/look polling and food tile search
##

from inventory import parse_inventory
from look import parse_look
from player_state import PlayerState
from sender import CommandSender

# Food threshold below which the player drops everything and forages.
CRITICAL_FOOD = 3

# Food threshold below which the player enters opportunistic foraging.
SAFE_FOOD = 15

# Number of FSM ticks between automatic Inventory polls when none is
# already in flight.
INVENTORY_POLL_INTERVAL = 20


def on_inventory_response(state: PlayerState, line: str) -> None:
    """Update inventory from a server Inventory response line."""
    state.pending_inventory = False
    parsed = parse_inventory(line)
    if parsed is not None:
        state.inventory = parsed


def on_look_response(state: PlayerState, line: str) -> None:
    """Update last_look from a server Look response line."""
    state.pending_look = False
    parsed = parse_look(line, state.level)
    if parsed is not None:
        state.last_look = parsed


def poll_inventory(state: PlayerState, sender: CommandSender,
                   tick: int) -> bool:
    """Issue an Inventory command if the poll interval has elapsed.

    Does nothing if an Inventory is already in flight or the pipeline
    is full. Returns True if sent.
    """
    if state.pending_inventory:
        return False
    if tick % INVENTORY_POLL_INTERVAL != 0:
        return False
    if not sender.can_send:
        return False
    state.pending_inventory = True
    sender.send("Inventory",
                callback=lambda line: on_inventory_response(state, line))
    return True


def find_food_tile(state: PlayerState) -> int | None:
    """Return the tile index of the nearest food tile in last_look, or None."""
    for idx, tile in enumerate(state.last_look):
        if "food" in tile:
            return idx
    return None


def request_look(state: PlayerState, sender: CommandSender) -> bool:
    """Issue a Look command if none is already in flight. Returns True if sent."""
    if state.pending_look or not sender.can_send:
        return False
    state.pending_look = True
    sender.send("Look",
                callback=lambda line: on_look_response(state, line))
    return True
