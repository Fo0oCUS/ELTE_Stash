import socket
import struct
import sys

server_addr = (sys.argv[1], int(sys.argv[2]))
packer = struct.Struct("4s i")
op, x = sys.argv[3], int(sys.argv[4])

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
    client.sendall(packer.pack(op.encode(), x))
    data = client.recv(16)
    print("Received:", data.decode())