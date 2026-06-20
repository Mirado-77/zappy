##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## Elevation response parsers and state tracking
##

from dataclasses import dataclass, field
from enum import Enum, auto


class ElevationStatus(Enum):
    """Possible states of an in-progress or recently completed elevation.

    Attributes:
        IDLE: No incantation is in progress.
        IN_PROGRESS: 'Elevation underway' was received; the player is frozen.
        SUCCESS: 'Current level: k' was received; the ritual succeeded.
        FAILED: 'ko' was received in the context of an incantation.
    """

    IDLE = auto()
    IN_PROGRESS = auto()
    SUCCESS = auto()
    FAILED = auto()


@dataclass
class ElevationState:
    """Mutable container for the current elevation tracking data.

    This object is shared between the parser callbacks and the FSM so
    that both can observe and update elevation progress.

    Attributes:
        status: Current elevation status.
        new_level: Level reached after a successful elevation (0 if none).
    """

    status: ElevationStatus = ElevationStatus.IDLE
    new_level: int = 0


def parse_current_level(line: str) -> int | None:
    """Extract the level number from a 'Current level: k' server line.

    Args:
        line: Raw server response line.

    Returns:
        The new level as an integer, or None if the line does not match
        the expected format or the level is not a positive integer.
    """
    prefix = "Current level: "
    if not line.startswith(prefix):
        return None
    raw = line[len(prefix):]
    try:
        level = int(raw)
    except ValueError:
        return None
    if level < 1:
        return None
    return level


def is_elevation_underway(line: str) -> bool:
    """Return True if the line is the 'Elevation underway' server message.

    Args:
        line: Raw server response line.

    Returns:
        True when the line exactly matches 'Elevation underway'.
    """
    return line.strip() == "Elevation underway"


def make_elevation_underway_handler(state: ElevationState):
    """Create a dispatcher handler that marks an elevation as in progress.

    Args:
        state: Shared ElevationState object to update.

    Returns:
        A handler callable suitable for Dispatcher.register.
    """
    def handler(_line: str) -> None:
        """Set elevation status to IN_PROGRESS.

        Args:
            _line: Full server line (always 'Elevation underway').
        """
        state.status = ElevationStatus.IN_PROGRESS

    return handler


def make_current_level_handler(state: ElevationState):
    """Create a dispatcher handler for 'Current level: k' responses.

    Args:
        state: Shared ElevationState object to update.

    Returns:
        A handler callable suitable for Dispatcher.register.
    """
    def handler(line: str) -> None:
        """Parse the new level and mark the elevation as successful.

        Args:
            line: Full server line, e.g. 'Current level: 3'.
        """
        level = parse_current_level(line)
        if level is not None:
            state.new_level = level
            state.status = ElevationStatus.SUCCESS

    return handler
