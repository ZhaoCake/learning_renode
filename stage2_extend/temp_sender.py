#!/usr/bin/env python3
import socket
import sys
import time

HOST = "127.0.0.1"
PORT = 5555


def send_values(values, interval=1.0, port=PORT):
    with socket.create_connection((HOST, port), timeout=5) as sock:
        for v in values:
            sock.sendall(f"{v}\n".encode())
            time.sleep(interval)


def main():
    if len(sys.argv) < 2:
        print("Usage: temp_sender.py <value1> [value2 ...] [--interval=seconds] [--port=5555]")
        return

    interval = 1.0
    vals = []
    port = PORT
    for arg in sys.argv[1:]:
        if arg.startswith("--interval="):
            interval = float(arg.split("=", 1)[1])
        elif arg.startswith("--port="):
            port = int(arg.split("=", 1)[1])
        else:
            vals.append(float(arg))

    if not vals:
        print("No values provided.")
        return

    send_values(vals, interval, port)


if __name__ == "__main__":
    main()
