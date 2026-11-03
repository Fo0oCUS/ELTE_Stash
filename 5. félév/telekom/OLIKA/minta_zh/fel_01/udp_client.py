import socket
import struct
import sys
from pathlib import Path
import random

request_packer = struct.Struct("20s i")                 
server_addr = (sys.argv[1], int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:

    # C rész
    data = {
        "1": ["almafa", 4],
        "2": ["kortefa", 3],
        "3": ["barackfa", 5]
    }

    index = str(random.randint(1, len(data)))
    text, num = data[index][0], data[index][1]
    client.sendto(request_packer.pack(text.encode(), num), server_addr)
    data, server_add = client.recvfrom(struct.Struct(f"{num}s").size)
    print(data.decode())