import socket
import sys
import struct

query = struct.Struct("!7H B6s B6s B 2H")
reply = struct.Struct("!7H B6s B6s B 2H HHHIH 4s")

server_address = (sys.argv[1], int(sys.argv[2]))
name = sys.argv[3]

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)

    s = name.split(".")

    client.sendall(query.pack(
        query.size - 2,
        1234,
        0x0100,
        1, 0, 0, 0,
        len(s[0]),
        s[0].encode(),
        len(s[1]),
        s[1].encode(),
        0,
        1, 1
    ))
    _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,address, = reply.unpack(client.recv(reply.size))

    print(socket.inet_ntoa(address))