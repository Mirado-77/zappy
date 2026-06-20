##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## FSM orchestrator: priority evaluator and main tick loop
##

from collecting import tick_collecting
from elevation import ElevationState
from foraging import tick_foraging
from fsm_helpers import CRITICAL_FOOD, SAFE_FOOD, poll_inventory
from have_broadcast import TeamMemberState, broadcast_have
from player_state import FsmState, PlayerState
from sender import CommandSender
from stones import needed_stones
from survival import tick_critical_survival


def evaluate_fsm_state(state: PlayerState) -> FsmState:
    """Return the highest-priority FSM state that applies this tick.

    Evaluates conditions in strict priority order. The ELEVATING state
    is set externally by the elevation dispatcher, not here.
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
    # Remaining conditions (COORDINATING, LEADING_ELEVATION, FORKING)
    # will be added in future steps.
    return FsmState.SCOUTING


class Fsm:
    """Priority-driven finite state machine for one AI player.

    Attributes:
        team_members: Last-known teammate states keyed by broadcast direction.
    """

    def __init__(self, state: PlayerState) -> None:
        """Bind the FSM to the player state."""
        self._state = state
        self._tick: int = 0
        self._roam_counter: list[int] = [0]
        self.team_members: dict[int, TeamMemberState] = {}

    def tick(self, sender: CommandSender, elev: ElevationState) -> None:
        """Run one FSM iteration.

        Called by the event loop after every server line is dispatched.
        """
        self._tick += 1
        poll_inventory(self._state, sender, self._tick)
        broadcast_have(self._state, sender, self._tick)

        new_state = evaluate_fsm_state(self._state)
        self._state.fsm_state = new_state

        if new_state == FsmState.CRITICAL_SURVIVAL:
            tick_critical_survival(self._state, sender)
        elif new_state == FsmState.FORAGING:
            tick_foraging(self._state, sender)
        elif new_state == FsmState.COLLECTING:
            tick_collecting(self._state, sender, self._roam_counter)
        # Additional state handlers will be wired in future steps.
