##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## COORDINATING state handler (follower role in a joint incantation)
##

from dataclasses import dataclass

from broadcast_protocol import (MSG_CANCEL, MSG_COMING, MSG_RALLY, MSG_READY,
                                format_msg, parse_msg)
from navigation import navigate_to
from player_state import FsmState, PlayerState
from sender import CommandSender


@dataclass(slots=True)
class RallyInfo:
    """Destination tile for the current follower rally.

    Attributes:
        tx: Target x coordinate (already wrapped to map bounds).
        ty: Target y coordinate (already wrapped to map bounds).
        level: Level required for the incantation.
    """

    tx: int
    ty: int
    level: int


def on_rally_received(
        state: PlayerState,
        rally_holder: list,
        direction: int,
        fields: tuple[str, ...],
        sender: CommandSender) -> None:
    """Process an incoming RALLY message (follower side).

    Accepts the rally only when the announced level matches the player's
    current level. Sets the rally_holder, transitions the FSM state to
    COORDINATING, and immediately broadcasts COMING as acknowledgement.

    RALLY field format: level:tx:ty
    """
    if len(fields) != 3:
        return
    try:
        level, tx, ty = int(fields[0]), int(fields[1]), int(fields[2])
    except ValueError:
        return
    if level != state.level:
        return   # not an incantation for our level

    rally_holder[0] = RallyInfo(
        tx=tx % state.map_width,
        ty=ty % state.map_height,
        level=level,
    )
    state.fsm_state = FsmState.COORDINATING
    if sender.can_send:
        sender.send(f"Broadcast {format_msg(MSG_COMING)}",
                    callback=lambda r: None)


def handle_coordination_broadcast(
        state: PlayerState,
        rally_holder: list,
        sender: CommandSender,
        direction: int,
        text: str) -> None:
    """Route a received broadcast to the appropriate coordination handler.

    Only RALLY is handled here. CANCEL clears any active rally.
    Other types (COMING, READY, HAVE) are ignored by this handler.
    """
    msg = parse_msg(text)
    if msg is None:
        return
    if msg.msg_type == MSG_RALLY:
        on_rally_received(state, rally_holder, direction, msg.fields, sender)
    elif msg.msg_type == MSG_CANCEL:
        rally_holder[0] = None


def tick_coordinating(
        state: PlayerState,
        sender: CommandSender,
        rally_holder: list) -> None:
    """Execute one FSM tick in the COORDINATING state (follower role).

    Priority actions:
    1. Already on the rally tile: broadcast READY and clear the rally.
    2. Not yet there: navigate towards the rally tile.
    """
    if not sender.can_send:
        return
    rally = rally_holder[0]
    if rally is None:
        return   # no active rally; evaluator will transition away

    if state.x == rally.tx and state.y == rally.ty:
        sender.send(f"Broadcast {format_msg(MSG_READY)}",
                    callback=lambda r: None)
        rally_holder[0] = None
        return

    navigate_to(state, sender, rally.tx, rally.ty)
