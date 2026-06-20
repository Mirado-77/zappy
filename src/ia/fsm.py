##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## FSM - priority evaluator, inventory polling, CRITICAL_SURVIVAL state
##

from dataclasses import dataclass, field

from broadcast_protocol import CoordMessage, MSG_HAVE, format_msg, parse_msg
from elevation import ElevationState
from inventory import parse_inventory
from look import parse_look, tile_index_to_offset
from navigation import navigate_to
from player_state import FsmState, PlayerState, apply_forward, apply_left, apply_right
from sender import CommandSender
from stones import needed_stones, STONE_NAMES, ELEVATION_REQUIREMENTS

# Food threshold below which the player drops everything and forages.
CRITICAL_FOOD = 3

# Food threshold below which the player enters opportunistic foraging.
SAFE_FOOD = 15

# Number of FSM ticks between automatic Inventory polls when none is
# already in flight.
INVENTORY_POLL_INTERVAL = 20

# Number of FSM ticks between HAVE broadcasts.
HAVE_BROADCAST_INTERVAL = 50

# HAVE field order: level then stones in STONE_NAMES order.
_HAVE_STONE_ORDER = ("linemate", "deraumere", "sibur", "mendiane",
                     "phiras", "thystame")


@dataclass(slots=True)
class TeamMemberState:
    """Last known state of one teammate as parsed from HAVE broadcasts.

    Attributes:
        level: Reported level.
        stones: Stone counts keyed by name.
        direction: Last broadcast direction (K value, 0 means same tile).
    """

    level: int
    stones: dict[str, int]
    direction: int


def _format_have(state: PlayerState) -> str:
    """Format a HAVE broadcast message for the current player state.

    The HAVE message encodes the player's level followed by each stone
    count in _HAVE_STONE_ORDER so teammates can determine how many stones
    this player contributes toward a joint incantation.

    Args:
        state: Current player state.

    Returns:
        A string ready for use as ``Broadcast <text>``.
    """
    fields = [str(state.level)]
    for stone in _HAVE_STONE_ORDER:
        fields.append(str(state.inventory.get(stone, 0)))
    return format_msg(MSG_HAVE, *fields)


def _parse_have(msg: CoordMessage) -> TeamMemberState | None:
    """Parse a CoordMessage of type HAVE into a TeamMemberState.

    Expected fields: level, linemate, deraumere, sibur, mendiane,
    phiras, thystame (7 integers in that order).

    Args:
        msg: A CoordMessage with msg_type == MSG_HAVE.

    Returns:
        TeamMemberState on success, None if the message is malformed.
    """
    if len(msg.fields) != 7:
        return None
    try:
        values = [int(f) for f in msg.fields]
    except ValueError:
        return None
    if values[0] < 1:
        return None
    stones = {name: values[i + 1] for i, name in enumerate(_HAVE_STONE_ORDER)}
    return TeamMemberState(level=values[0], stones=stones, direction=0)


def on_broadcast_received(
        team_members: dict[int, TeamMemberState],
        direction: int,
        text: str) -> None:
    """Process an incoming broadcast and update the team member table.

    Only HAVE messages are handled here. Other message types are handled
    by dedicated coordination handlers in future steps.

    Each distinct broadcast direction is treated as a separate teammate
    so the table is indexed by the direction value (1-8). This is a
    simplified proxy for player identity; in practice two teammates may
    share the same direction temporarily.

    Args:
        team_members: Mutable dict mapping direction -> TeamMemberState.
        direction: K value from the broadcast (0 = same tile, 1-8 = direction).
        text: Raw text portion of the broadcast line.
    """
    msg = parse_msg(text)
    if msg is None:
        return
    if msg.msg_type == MSG_HAVE:
        member = _parse_have(msg)
        if member is not None:
            member.direction = direction
            team_members[direction] = member


def _broadcast_have(state: PlayerState, sender: CommandSender,
                    tick: int) -> bool:
    """Emit a HAVE broadcast if the broadcast interval has elapsed.

    Does nothing if the sender pipeline is full.

    Args:
        state: Current player state.
        sender: Command sender.
        tick: Current FSM tick counter.

    Returns:
        True if the command was sent.
    """
    if tick % HAVE_BROADCAST_INTERVAL != 0:
        return False
    if not sender.can_send:
        return False
    text = _format_have(state)
    sender.send(f"Broadcast {text}", callback=lambda r: None)
    return True


def _on_inventory_response(state: PlayerState, line: str) -> None:
    """Callback invoked when the server responds to an Inventory command.

    Updates the player's inventory in place and clears the in-flight flag.

    Args:
        state: Mutable player state to update.
        line: Raw server response line.
    """
    state.pending_inventory = False
    parsed = parse_inventory(line)
    if parsed is not None:
        state.inventory = parsed


def _on_look_response(state: PlayerState, line: str) -> None:
    """Callback invoked when the server responds to a Look command.

    Updates the player's last_look in place and clears the in-flight flag.

    Args:
        state: Mutable player state to update.
        line: Raw server response line.
    """
    state.pending_look = False
    parsed = parse_look(line, state.level)
    if parsed is not None:
        state.last_look = parsed


def _poll_inventory(state: PlayerState, sender: CommandSender,
                    tick: int) -> bool:
    """Issue an Inventory command if the poll interval has elapsed.

    Does nothing if an Inventory is already in flight or the sender
    pipeline is full.

    Args:
        state: Mutable player state.
        sender: Command sender.
        tick: Current FSM tick counter.

    Returns:
        True if the command was sent.
    """
    if state.pending_inventory:
        return False
    if tick % INVENTORY_POLL_INTERVAL != 0:
        return False
    if not sender.can_send:
        return False
    state.pending_inventory = True
    sender.send("Inventory",
                callback=lambda line: _on_inventory_response(state, line))
    return True


def _find_food_tile(state: PlayerState) -> int | None:
    """Return the index of the nearest tile containing food in last_look.

    Searches the Look result in tile-index order (lowest = nearest).
    Tile 0 (own tile) is included.

    Args:
        state: Player state with a valid last_look.

    Returns:
        Tile index of the first food-bearing tile, or None if no food
        is visible.
    """
    for idx, tile in enumerate(state.last_look):
        if "food" in tile:
            return idx
    return None


def _request_look(state: PlayerState, sender: CommandSender) -> bool:
    """Issue a Look command if none is already in flight.

    Args:
        state: Mutable player state.
        sender: Command sender.

    Returns:
        True if the command was sent.
    """
    if state.pending_look or not sender.can_send:
        return False
    state.pending_look = True
    sender.send("Look",
                callback=lambda line: _on_look_response(state, line))
    return True


def _tick_critical_survival(state: PlayerState,
                             sender: CommandSender) -> None:
    """Execute one FSM tick in the CRITICAL_SURVIVAL state.

    Priority actions in order:
    1. If food is on the current tile (tile 0), take it immediately.
    2. If a Look result is available and food is visible, navigate there.
    3. If no Look result is available, request one.
    4. If nothing is visible, move forward one step to explore.

    Args:
        state: Mutable player state.
        sender: Command sender.
    """
    if not sender.can_send:
        return

    # 1. Food on current tile: take it right now.
    if state.last_look and "food" in state.last_look[0]:
        sender.send("Take food",
                    callback=lambda r: None)
        return

    # 2. Food visible elsewhere: navigate there.
    if state.last_look:
        food_idx = _find_food_tile(state)
        if food_idx is not None and food_idx > 0:
            dx, dy = tile_index_to_offset(food_idx, state.level)
            tx = (state.x + dx) % state.map_width
            ty = (state.y + dy) % state.map_height
            navigate_to(state, sender, tx, ty)
            return

    # 3. No look result yet: request one.
    if not state.last_look:
        _request_look(state, sender)
        return

    # 4. Nothing visible: advance one tile and request a new look.
    sender.send("Forward",
                callback=lambda r: apply_forward(state) if r == "ok" else None)
    _request_look(state, sender)


# Roaming turn interval: turn right every N forward steps to spiral.
_ROAM_TURN_INTERVAL = 4


def _find_needed_stone_tile(
        state: PlayerState,
        wanted: dict[str, int]) -> tuple[int, str] | None:
    """Find the nearest tile containing a stone the player still needs.

    Scans the Look result in tile-index order (lowest = nearest). Only
    returns a stone that is in the wanted dict. Skips stones that would
    exceed the needed quantity (over-collection prevention).

    Args:
        state: Current player state with a valid last_look.
        wanted: Dict of {resource: quantity_still_needed}.

    Returns:
        (tile_index, stone_name) of the nearest match, or None.
    """
    for idx, tile in enumerate(state.last_look):
        for stone in STONE_NAMES:
            if stone in tile and stone in wanted:
                return idx, stone
    return None


def _tick_collecting(state: PlayerState, sender: CommandSender,
                     roam_counter: list[int]) -> None:
    """Execute one FSM tick in the COLLECTING state.

    Priority actions:
    1. Needed stone on current tile: take it (over-collection check applied).
    2. Needed stone visible in Look result: navigate there.
    3. Look not yet requested: request it.
    4. Nothing useful visible: roam (forward + periodic turn).

    The roam_counter list holds a single integer so it can be mutated
    in place across ticks without a class attribute.

    Args:
        state: Mutable player state.
        sender: Command sender.
        roam_counter: Single-element list used as a mutable roam step counter.
    """
    if not sender.can_send:
        return

    wanted = needed_stones(state.level, state.inventory)
    if not wanted:
        return   # nothing left to collect; evaluator will transition state

    # 1. Needed stone on own tile.
    if state.last_look:
        for stone in STONE_NAMES:
            if stone in state.last_look[0] and stone in wanted:
                sender.send(f"Take {stone}", callback=lambda r: None)
                return

    # 2. Needed stone visible elsewhere.
    if state.last_look:
        match = _find_needed_stone_tile(state, wanted)
        if match is not None:
            idx, stone = match
            dx, dy = tile_index_to_offset(idx, state.level)
            tx = (state.x + dx) % state.map_width
            ty = (state.y + dy) % state.map_height
            navigate_to(state, sender, tx, ty)
            return

    # 3. No Look result: request one.
    if not state.last_look:
        _request_look(state, sender)
        return

    # 4. Nothing visible: roam with periodic turns.
    roam_counter[0] += 1
    if roam_counter[0] % _ROAM_TURN_INTERVAL == 0:
        sender.send("Right",
                    callback=lambda r: apply_right(state) if r == "ok" else None)
    else:
        sender.send("Forward",
                    callback=lambda r: apply_forward(state) if r == "ok" else None)
    state.last_look = []   # invalidate so a fresh Look is requested next tick
    _request_look(state, sender)


def _tick_foraging(state: PlayerState, sender: CommandSender) -> None:
    """Execute one FSM tick in the FORAGING state.

    Opportunistically collects food when below the safe threshold.
    Does not abandon navigation or coordination in progress; yields
    immediately to CRITICAL_SURVIVAL if food drops further.

    Priority actions:
    1. Food on current tile: take it.
    2. Food visible one step away in the Look result: navigate and take.
    3. No food visible: advance one step then request a new Look.

    Args:
        state: Mutable player state.
        sender: Command sender.
    """
    if not sender.can_send:
        return

    # 1. Food on own tile.
    if state.last_look and "food" in state.last_look[0]:
        sender.send("Take food", callback=lambda r: None)
        return

    # 2. Food visible nearby (only one step away, tile index 1-3 at level 1).
    if state.last_look:
        food_idx = _find_food_tile(state)
        if food_idx is not None and food_idx > 0:
            dx, dy = tile_index_to_offset(food_idx, state.level)
            tx = (state.x + dx) % state.map_width
            ty = (state.y + dy) % state.map_height
            navigate_to(state, sender, tx, ty)
            return

    # 3. Nothing visible: move forward and refresh look.
    sender.send("Forward",
                callback=lambda r: apply_forward(state) if r == "ok" else None)
    _request_look(state, sender)


def _evaluate_fsm_state(state: PlayerState) -> FsmState:
    """Determine the highest-priority FSM state that applies right now.

    Evaluates conditions in strict priority order and returns the first
    matching state. The ELEVATING state is not set here; it is set by
    the elevation handlers in event_loop.py when the server sends
    'Elevation underway'.

    Args:
        state: Current player state.

    Returns:
        The FSM state the player should be in this tick.
    """
    food = state.inventory.get("food", 0)

    if food < CRITICAL_FOOD:
        return FsmState.CRITICAL_SURVIVAL
    if state.fsm_state == FsmState.ELEVATING:
        return FsmState.ELEVATING
    if needed_stones(state.level, state.inventory):
        return FsmState.COLLECTING
    if food < SAFE_FOOD:
        return FsmState.FORAGING
    # Remaining states will be filled in by future steps.
    # Default to SCOUTING for now.
    return FsmState.SCOUTING


class Fsm:
    """Priority-driven finite state machine for one AI player.

    Created once after the handshake and called on every event loop tick.

    Attributes:
        _state: Shared mutable player state.
        _tick: FSM iteration counter used for periodic polling.
    """

    def __init__(self, state: PlayerState) -> None:
        """Bind the FSM to the player state.

        Args:
            state: Mutable player state owned by this FSM.
        """
        self._state = state
        self._tick: int = 0
        self._roam_counter: list[int] = [0]
        self.team_members: dict[int, TeamMemberState] = {}

    def tick(self, sender: CommandSender, elev: ElevationState) -> None:
        """Run one FSM iteration.

        Called by the event loop after every server line is dispatched.
        Polls the inventory periodically, evaluates the priority chain,
        and executes the appropriate state handler.

        Args:
            sender: Command sender for enqueuing commands.
            elev: Shared elevation state updated by the dispatcher.
        """
        self._tick += 1
        _poll_inventory(self._state, sender, self._tick)
        _broadcast_have(self._state, sender, self._tick)

        new_state = _evaluate_fsm_state(self._state)
        self._state.fsm_state = new_state

        if new_state == FsmState.CRITICAL_SURVIVAL:
            _tick_critical_survival(self._state, sender)
        elif new_state == FsmState.FORAGING:
            _tick_foraging(self._state, sender)
        elif new_state == FsmState.COLLECTING:
            _tick_collecting(self._state, sender, self._roam_counter)
        # Additional state handlers will be wired in future steps.
