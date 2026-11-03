import socket
import struct
import sys

server_in_packer = struct.Struct("6s 8s 7s")
server_out_packer = struct.Struct("i 10s")
server_addr = (sys.argv[1], int(sys.argv[2]))

neptun = "M4WZZO"
jaratszam = "LH2048"


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_addr)
    client.sendall(server_in_packer.pack(neptun.encode(), jaratszam.encode(), b"request"))
    data = recv_exact(client, server_out_packer.size)
    helyek, osztaly = server_out_packer.unpack(data)

    if(helyek < 3 or osztaly.rstrip(b"\x00") != b"economy"):
        client.sendall(server_in_packer.pack(neptun.encode(), jaratszam.encode(), b"reject"))
    else:
        client.sendall(server_in_packer.pack(neptun.encode(), jaratszam.encode(), b"reserve"))

    data = recv_exact(client, struct.Struct("12s").size)
    data, = struct.unpack("12s", data)
    print(data.decode())
