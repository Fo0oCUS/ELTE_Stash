import socket
import sys
import struct

packer = struct.Struct("i i c")
server_address = (sys.argv[1],int(sys.argv[2]))
a = int(sys.argv[3])
op = sys.argv[4]
b = int(sys.argv[5])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    package = packer.pack(a,b,op.encode())

    client.sendall(package)

    recv = client.recv(packer.size)
    a,b,op = packer.unpack(recv)

    print(f"Result: {a}")



