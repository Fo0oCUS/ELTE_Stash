import sys
import struct
import socket

server_address = (sys.argv[1], int(sys.argv[2]))
query = struct.Struct("!7H B6s B6s B 2H")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    client.sendall(
        query.pack(
            query.size - 2,
            1234,
            0x0100,
            1, 0, 0, 0,
            6, b"D6018J",
            6, b"telkom",
            0,
            1, 1
        )
    )

    data = b""

    while len(data) < 49:
        chunk = client.recv(49 - len(data))
        if not chunk:
            raise ConnectionError("Incomplete DNS response")
        data += chunk

    ip = socket.inet_ntoa(data[-4:])
    print(ip)