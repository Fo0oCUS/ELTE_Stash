import socket
import struct
import sys
import select

packer = struct.Struct("i i c")

proxy_address = ("localhost",int(sys.argv[1]))
server_address = ("localhost",int(sys.argv[2]))


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy,\
    socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:

    server.connect(server_address)

    proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy.bind(proxy_address)
    proxy.listen(5)
    print("Proxy is running:", proxy_address, "->",server_address)

    inputs = [proxy]
    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is proxy:
                client, addr = proxy.accept()
                inputs.append(client)
                print("Új kliens csatlakozott", addr)
                continue

            msg = recv_exact(s, packer.size)

            if not msg:
                inputs.remove(s)
                s.close()
                continue

            server.sendto(msg,server_address)

            data, server_addr = server.recvfrom(1024)
            s.sendall(data)



                