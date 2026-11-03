import socket
import struct
import sys

packer = struct.Struct("i i c")
host, port = (sys.argv[1], int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
    client.sendto(packer.pack(a, b, op.encode()), (host, port))
    data, address = client.recvfrom(2048)
    result, _, _ = packer.unpack(data)