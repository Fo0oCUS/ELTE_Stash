
import sys
import struct
import socket

packer = struct.Struct("ii1s")

server_address = (sys.argv[1], int(sys.argv[2]))
a = int(sys.argv[3])
op = sys.argv[4]
b = int(sys.argv[5])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)

    client.sendall(packer.pack(a, b, op.encode()))

    data = b""
    while len(data) < packer.size:
        chunk = client.recv(packer.size - len(data))
        if not chunk:
            break
        data += chunk

    if len(data) == packer.size:
        result, error, status = packer.unpack(data)

        if status == b'R':
            print(result)
        else:
            print("Error")
    else:
        print("Invalid server response")
