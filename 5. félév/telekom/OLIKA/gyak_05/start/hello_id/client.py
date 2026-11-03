import socket
import struct
import sys

packer = struct.Struct("i12s")                    # egyelore csak a 'Hello server' uzenet
server_addr = (sys.argv[1], int(sys.argv[2]))   # pl. python client.py localhost 10000
id = int(sys.argv[3])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_addr)
    client.sendall(packer.pack(id, "Hello server".encode()))
    data = client.recv(64)
    print("Received:", data.decode())
