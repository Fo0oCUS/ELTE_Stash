import select
import socket
import struct
import sys

proxy_address = ("localhost",int(sys.argv[1]))
server_address = ("localhost",int(sys.argv[2]))


request_packer =  struct.Struct("3s")
reply_packer = struct.Struct("i")

known = {}

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy,\
    socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.connect(server_address)

    proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy.bind(proxy_address)
    proxy.listen(5)

    inputs = [proxy]
    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is proxy:
                client, client_address = proxy.accept()
                print("Egy klines csatlakozott")
                inputs.append(client)
                continue

            msg = recv_exact(s,request_packer.size)

            if not msg:
                print("Egy kliens bontott")
                inputs.remove(s)
                s.close()
                continue

            if msg not in known:
                server.sendall(msg)
                data = recv_exact(server,reply_packer.size)
                known[msg] = data
            
            s.sendall(known[msg])
            
            