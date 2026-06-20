##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## LEADING_ELEVATION state handler (leader role in a joint incantation)
##

from dataclasses import dataclass, field

from broadcast_protocol import MSG_CANCEL, MSG_RALLY, MSG_READY, format_msg, parse_msg
from elevation import ElevationState, ElevationStatus
from player_state import FsmState, PlayerState
from sender import CommandSender
from stones import ELEVATION_REQUIREMENTS, STONE_NAMES, needed_stones

# Maximum ticks to wait for all READY replies before cancelling.
RALLY_TIMEOUT_TICKS = 60


@dataclass(slots=True)
class LeaderInfo:
    """State tracked by the leader during a rally-and-incantation sequence.

    Attributes:
        ready_count: Number of READY replies received so far.
        ticks_waiting: Ticks elapsed since the RALLY was broadcast.
    """

    ready_count: int = 0
    ticks_waiting: int = 0


def can_lead_elevation(state: PlayerState) -> bool:
    """Return True when this player can initiate an incantation as leader.

    Conditions (all must hold):
    - No stones are still needed (inventory is satisfied for this level).
    - The player's own stones alone satisfy the stone requirements.
      (They will be deposited on the tile before Incantation.)
    - Level 1 requires only 1 player, so no coordination is needed.
    """
    if needed_stones(state.level, state.inventory):
        return False
    req = ELEVATION_REQUIREMENTS.get(state.level, {})
    needed_players = req.get("players", 0)
    # For level 1, only 1 player needed: leader can act alone.
    return needed_players >= 1


def deposit_stones(state: PlayerState, sender: CommandSender) -> int:
    """Enqueue Set commands to deposit all required stones on the tile.

    Only deposits stones listed in the elevation requirements for this level
    (does not drop surplus stones). Returns the number of commands sent.
    """
    req = ELEVATION_REQUIREMENTS.get(state.level, {})
    sent = 0
    for stone in STONE_NAMES:
        qty = req.get(stone, 0)
        for _ in range(qty):
            if not sender.can_send:
                break
            sender.send(f"Set {stone}", callback=lambda r: None)
            sent += 1
    return sent


def required_ready_count(level: int) -> int:
    """Return the number of READY replies needed before Incantation.

    Leader counts as 1, so followers needed = players_required - 1.
    """
    req = ELEVATION_REQUIREMENTS.get(level, {})
    return max(0, req.get("players", 1) - 1)


def on_ready_received(
        state: PlayerState,
        leader_info: list,
        sender: CommandSender) -> None:
    """Process a READY broadcast from a follower.

    Increments the ready counter. If enough followers are ready and the
    inventory is still satisfied, deposits stones and starts Incantation.
    Resets leader_info[0] to None so the leader state clears.
    """
    info = leader_info[0]
    if info is None:
        return
    info.ready_count += 1
    needed = required_ready_count(state.level)
    if info.ready_count >= needed:
        deposit_stones(state, sender)
        if sender.can_send:
            sender.send("Incantation", callback=lambda r: None)
        leader_info[0] = None


def handle_leader_broadcast(
        state: PlayerState,
        leader_info: list,
        sender: CommandSender,
        text: str) -> None:
    """Route a received broadcast to the leader-side handler.

    Only READY is relevant here. All other types are ignored.
    """
    msg = parse_msg(text)
    if msg is None:
        return
    if msg.msg_type == MSG_READY:
        on_ready_received(state, leader_info, sender)


def tick_leading_elevation(
        state: PlayerState,
        sender: CommandSender,
        leader_info: list) -> None:
    """Execute one FSM tick in the LEADING_ELEVATION state.

    Actions on the first tick: broadcast RALLY with own coordinates.
    Subsequent ticks: count READY replies (handled via on_ready_received).
    If the timeout elapses: broadcast CANCEL and abort.
    """
    info = leader_info[0]
    if info is None:
        return   # rally completed or cancelled; evaluator transitions state

    if not sender.can_send:
        return

    # First tick: send the RALLY broadcast.
    if info.ticks_waiting == 0 and required_ready_count(state.level) > 0:
        text = format_msg(MSG_RALLY, str(state.level),
                          str(state.x), str(state.y))
        sender.send(f"Broadcast {text}", callback=lambda r: None)
    elif info.ticks_waiting == 0:
        # Level 1: no followers needed; deposit and incantate immediately.
        deposit_stones(state, sender)
        if sender.can_send:
            sender.send("Incantation", callback=lambda r: None)
        leader_info[0] = None
        return

    info.ticks_waiting += 1
    if info.ticks_waiting > RALLY_TIMEOUT_TICKS:
        sender.send(f"Broadcast {format_msg(MSG_CANCEL)}",
                    callback=lambda r: None)
        leader_info[0] = None
