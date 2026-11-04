import sys
import socket
import select
import struct


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

in_packer = struct.Struct("i 20s")
out_packer = struct.Struct("5s 20s")

proxy_address = ("localhost",int(sys.argv[1]))
server_addres = ("localhost",int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy,\
    socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:

    proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy.bind(proxy_address)
    proxy.listen(5)

    inputs = [ proxy ]
    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is proxy:
                client, _ = proxy.accept()
                inputs.append(client)
                continue

            msg = recv_exact(s,in_packer.size)

            if not msg:
                inputs.remove(s)
                s.close()
                continue

            client_type, username = in_packer.unpack(msg)

            i = b"LOGIN" if client_type == 0 else b"GUEST"

            server.sendto(out_packer.pack(i,username),server_addres)

            data, _ = server.recvfrom(1024)

            s.sendall(data)





