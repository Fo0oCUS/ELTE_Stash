import sys
import socket
import struct
import math

client_packer = struct.Struct("6s i i")
server_packer = struct.Struct("c 10s")
server_address = (sys.argv[1], int(sys.argv[2]))
neptun = sys.argv[3]
counter = 1
low = 1
high = 100



with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    while low <= high:
        mid = low + (high - low) // 2

        client.sendall(client_packer.pack(neptun.encode(), mid, counter))
        counter += 1
        (op, id) = server_packer.unpack(client.recv(server_packer.size))
        op = op.decode()
        if op == "<":
            high = mid - 1
        elif op == ">":
            low = mid + 1 
        elif op == "=":
            client.sendall(struct.pack("6s 10s i", neptun.encode(), id, counter))
            kod = client.recv(12)
            print(kod.decode())
            break