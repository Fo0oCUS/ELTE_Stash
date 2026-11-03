import socket
import sys
import struct

packer = struct.Struct("20s i")
server_address = (sys.argv[1],int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)

    client.sendall(packer.pack("alma".encode(),2))

    print(f"Recieved Message: {client.recv(packer.size).decode()}")



    