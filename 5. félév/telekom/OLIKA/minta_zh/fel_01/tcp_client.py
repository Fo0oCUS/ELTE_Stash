import socket
import struct
import sys
from pathlib import Path
import random

request_packer = struct.Struct("20s i")                 
server_addr = (sys.argv[1], int(sys.argv[2]))

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

    # A rész
    # text, num = input().split()
    # num = int(num)
    # client.sendall(request_packer.pack(text.encode(), num))
    # data = recv_exact(client, struct.Struct(f"{num}s").size)
    # print(data.decode())

    # B rész
    # data = []
    # path = Path(__file__).parent / "input.txt"
    # with open(path, "r") as f:
    #     for line in f:
    #         data.append(line.rstrip("\n"))

    # index = random.randint(0, len(data) -1)
    # text, num = data[index].split()[0], int(data[index].split()[1])
    # client.sendall(request_packer.pack(text.encode(), num))
    # data = recv_exact(client, struct.Struct(f"{num}s").size)
    # print(data.decode())

    # C rész
    data = {
        "1": ["almafa", 4],
        "2": ["kortefa", 3],
        "3": ["barackfa", 5]
    }

    index = str(random.randint(1, len(data)))
    text, num = data[index][0], data[index][1]
    client.sendall(request_packer.pack(text.encode(), num))
    data = recv_exact(client, struct.Struct(f"{num}s").size)
    print(data.decode())