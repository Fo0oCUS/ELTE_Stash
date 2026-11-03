import socket
import struct
import sys

packer = struct.Struct("i i c") 
server_addr = ("", int(sys.argv[1])) 

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_addr)
    server.settimeout(1.0) 

    try:
        while True:
            try:
                data, client = server.recvfrom(2048)
                a, b, op = packer.unpack(data)
                result = ops[op](a, b)
                print(f"{a} {op.decode()} {b} = {result}")
                server.sendto(packer.pack(result, 0, b"R"), client)
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")