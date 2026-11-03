import socket
import sys
import struct

server_address = (sys.argv[1],int(sys.argv[2]))
packer = struct.Struct("6s i i")
unpacker = struct.Struct("c 10s")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    neptun_code = "hwo3yf"
    tries = 1
    guess = 50
    max = 100
    min = 1
    client.sendall(packer.pack(neptun_code.encode(),guess,tries))

    c,code = unpacker.unpack(client.recv(unpacker.size))

    while c != b"=":
        if c == b"<":
            max = guess
            guess = int((min + guess) / 2)
        else:
            min = guess
            guess = int((max + guess) / 2)

        tries += 1
        client.sendall(packer.pack(neptun_code.encode(),guess,tries))

        c,code = unpacker.unpack(client.recv(unpacker.size))

    client.sendall(struct.pack("6s 10s i",neptun_code.encode(),code,tries + 1))

    code = client.recv(struct.calcsize("12s")).decode().strip("\x00")
    print(code)
        








