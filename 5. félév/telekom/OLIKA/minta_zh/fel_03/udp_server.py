import sys
import socket
import struct
import random

server_address = ("localhost", int(sys.argv[1]))
request_packer = struct.Struct("5s i")

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.settimeout(1)

    value = 0

    try:
        while True:
            try:
                    data, client_address = server.recvfrom(4096)
                    print(f"Connected: {client_address[0]}:{client_address[1]}")

                    op, x = request_packer.unpack(data)
                    op = op.rstrip(b"\x00")

                    match op:
                        case b"PUSH": value = x
                        case b"PLUS": value += x
                        case b"MINUS": value -= x
                        case _: continue

                    value += random.randint(-1, 1) # B rész hibázás
                    server.sendto(struct.pack("i", value), client_address)
                        
            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Server stopped")