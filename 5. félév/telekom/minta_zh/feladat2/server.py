import socket
import sys
import select
import struct


packer = struct.Struct("3s 6s 8s i")
server_address = ("",int(sys.argv[1]))


dict = {}

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket. SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    server.bind(server_address)
    server.listen(5)

    inputs = [server]
    while True:
        readables, _ , _ = select.select(inputs, [], [] , 1.0)
        for s in readables:
            if s is server:
                client, client_address = s.accept()
                inputs.append(client)
                continue

            msg = recv_exact(s,packer.size)

            if not msg:
                inputs.remove(s)
                s.close()
                continue

            f, w, p, q = packer.unpack(msg)

            match f:
                case b"ADD":
                    if w not in dict:
                        dict[w] = {}
        
                    if p in dict[w]:
                        dict[w][p] += q
                    else:
                        dict[w][p] = q
                    s.sendall(struct.pack("i",dict[w][p]))
                case b"GET":
                    if w in dict:
                        if p in dict[w]:
                            s.sendall(struct.pack("i",dict[w][p]))
                case b"TOT":
                    if w in dict:
                        s.sendall(struct.pack("i",sum(dict[w].values())))




    
