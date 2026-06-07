#!/usr/bin/env python3

##
## EPITECH PROJECT, 2026
## local_zappy
## File description:
## main
##

import sys


USAGE = "USAGE: ./zappy_ai -p port -n name [-h machine]"
ERROR = 84


def parse_args(argv: list[str]) -> tuple[int, str, str]:
    """Parse command-line arguments.

    Returns:
        (port, name, machine)

    Exits with code 84 on missing, duplicate, or invalid arguments.
    Exits with code 0 and prints usage to stderr on --help.
    """
    if "--help" in argv:
        print(USAGE, file=sys.stderr)
        sys.exit(0)

    port: int | None = None
    name: str | None = None
    machine: str = "localhost"

    i = 0
    while i < len(argv):
        flag = argv[i]
        if flag in ("-p", "-n", "-h"):
            if i + 1 >= len(argv):
                print(f"error: flag {flag} requires an argument", file=sys.stderr)
                print(USAGE, file=sys.stderr)
                sys.exit(ERROR)
            value = argv[i + 1]
            i += 2
            if flag == "-p":
                if port is not None:
                    print("error: duplicate flag -p", file=sys.stderr)
                    sys.exit(ERROR)
                try:
                    port = int(value)
                except ValueError:
                    print(f"error: port must be an integer, got '{value}'",
                          file=sys.stderr)
                    sys.exit(ERROR)
                if not (1 <= port <= 65535):
                    print(f"error: port must be in range 1-65535, got {port}",
                          file=sys.stderr)
                    sys.exit(ERROR)
            elif flag == "-n":
                if name is not None:
                    print("error: duplicate flag -n", file=sys.stderr)
                    sys.exit(ERROR)
                if not value:
                    print("error: team name must not be empty", file=sys.stderr)
                    sys.exit(ERROR)
                name = value
            elif flag == "-h":
                machine = value
        else:
            print(f"error: unknown argument '{flag}'", file=sys.stderr)
            print(USAGE, file=sys.stderr)
            sys.exit(ERROR)

    if port is None:
        print("error: missing required flag -p", file=sys.stderr)
        print(USAGE, file=sys.stderr)
        sys.exit(ERROR)
    if name is None:
        print("error: missing required flag -n", file=sys.stderr)
        print(USAGE, file=sys.stderr)
        sys.exit(ERROR)

    return port, name, machine


if __name__ == "__main__":
    port, name, machine = parse_args(sys.argv[1:])
    print(f"port={port} name={name!r} machine={machine!r}")
