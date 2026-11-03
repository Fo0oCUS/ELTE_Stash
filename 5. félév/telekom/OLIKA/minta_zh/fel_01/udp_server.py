import sys
import socket
import struct

request_packer = struct.Struct("20s i")
server_address = ("localhost", int(sys.argv[1]))

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_address)
    sock.settimeout(1)

    try:
        while True:
            try:
                data, client = sock.recvfrom(request_packer.size)
                text, num = request_packer.unpack(data)
                new = text.decode()[:num][::-1]
                sock.sendto(struct.pack(f"{num}s", new.encode()), client)
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")