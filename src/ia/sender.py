##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## Command sender with in-flight queue tracking
##

import socket
import sys
from collections import deque
from typing import Callable

ERROR = 84
MAX_IN_FLIGHT = 10


class CommandSender:
    """Sends commands to the server and tracks how many are in flight.

    The server accepts at most MAX_IN_FLIGHT (10) commands without a
    response before it starts discarding new ones. This class enforces
    that limit and maintains a FIFO queue of pending callbacks that are
    resolved in order as server responses arrive.

    Attributes:
        _sock: Connected TCP socket used for sending.
        _in_flight: Number of commands sent but not yet acknowledged.
        _callbacks: Ordered queue of callables, one per in-flight command.
            Each callable receives the raw response line as its argument.
    """

    def __init__(self, sock: socket.socket) -> None:
        """Initialise the sender with a connected socket.

        Args:
            sock: A connected, blocking TCP socket.
        """
        self._sock = sock
        self._in_flight: int = 0
        self._callbacks: deque[Callable[[str], None]] = deque()

    @property
    def in_flight(self) -> int:
        """Number of commands sent but not yet acknowledged by the server.

        Returns:
            Current in-flight count (0 to MAX_IN_FLIGHT).
        """
        return self._in_flight

    @property
    def can_send(self) -> bool:
        """Whether the sender is allowed to enqueue another command.

        Returns:
            True if fewer than MAX_IN_FLIGHT commands are in flight.
        """
        return self._in_flight < MAX_IN_FLIGHT

    def send(self, command: str,
             callback: Callable[[str], None] | None = None) -> bool:
        """Send a command to the server if the in-flight limit allows it.

        The command string is transmitted terminated by a newline. An
        optional callback is registered and will be invoked with the
        server's response line when ``on_response`` is called.

        Args:
            command: Command text without the trailing newline.
            callback: Optional callable invoked with the response line
                when the server acknowledges this command.

        Returns:
            True if the command was sent, False if the in-flight queue
            is full and the command was not sent.
        """
        if not self.can_send:
            return False
        self._transmit(command)
        self._callbacks.append(callback if callback is not None else _noop)
        self._in_flight += 1
        return True

    def on_response(self, line: str) -> None:
        """Notify the sender that the server sent a response line.

        Decrements the in-flight counter and invokes the callback
        associated with the oldest pending command.

        Args:
            line: The raw response line received from the server.
        """
        if not self._callbacks:
            return
        self._in_flight -= 1
        cb = self._callbacks.popleft()
        cb(line)

    def _transmit(self, command: str) -> None:
        """Write a command line to the socket.

        Args:
            command: Command text without the trailing newline.
        """
        try:
            self._sock.sendall((command + "\n").encode())
        except OSError as exc:
            print(f"error: cannot send command '{command}': {exc}",
                  file=sys.stderr)
            sys.exit(ERROR)


def _noop(_line: str) -> None:
    """No-operation callback used when the caller does not need the response.

    Args:
        _line: Ignored response line.
    """
