import socket
import struct
import sys

packer = struct.Struct("i12s")
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
    client.sendall(packer.pack("Hello server".encode()))
    data = recv_exact(client, data.size)
    print("Received:", data.decode())