##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## Main event loop
##

import sys
from typing import Callable

from dispatcher import Dispatcher
from line_buffer import LineBuffer
from sender import CommandSender

FsmTickFn = Callable[[CommandSender], None]


def _handle_dead(_line: str) -> None:
    """Handle the 'dead' message from the server.

    The server sends 'dead' when the player has run out of food.
    The process must terminate immediately.

    Args:
        _line: The full line (always 'dead'), ignored.
    """
    sys.exit(0)


def _on_connection_closed() -> None:
    """Called when the server closes the TCP connection unexpectedly.

    Prints a notice to stderr and exits cleanly.
    """
    print("connection closed by server", file=sys.stderr)
    sys.exit(0)


def _build_dispatcher(sender: CommandSender) -> Dispatcher:
    """Construct the dispatcher and register built-in handlers.

    At this stage only the 'dead' handler is registered. Future steps
    will add handlers for broadcast messages, elevation responses, etc.
    All other lines fall through to the sender's on_response, which
    resolves the oldest pending command callback.

    Args:
        sender: The command sender whose on_response acts as the fallback.

    Returns:
        A configured Dispatcher instance.
    """
    d = Dispatcher(fallback=sender.on_response)
    d.register("dead", _handle_dead)
    return d


def run(buf: LineBuffer, sender: CommandSender,
        fsm_tick: FsmTickFn) -> None:
    """Run the main event loop until the session ends.

    Each iteration:
        1. Block until the next complete line arrives from the server.
        2. Dispatch the line to its registered handler.
        3. Call fsm_tick so the FSM can evaluate its state and send commands.

    The loop exits (via sys.exit) on:
        - Receipt of 'dead' from the server.
        - The server closing the TCP connection.

    Args:
        buf: Line-buffered socket reader.
        sender: Command sender with in-flight tracking.
        fsm_tick: Callable invoked once per loop iteration after the line
            has been dispatched. Receives the sender so it can enqueue
            new commands.
    """
    dispatcher = _build_dispatcher(sender)

    while True:
        line = buf.readline()
        if line is None:
            _on_connection_closed()
            return
        dispatcher.dispatch(line)
        fsm_tick(sender)
