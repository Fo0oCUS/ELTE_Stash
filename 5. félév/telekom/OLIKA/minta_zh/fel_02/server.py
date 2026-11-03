import sys
import socket
import select
import struct

server_address = ("localhost", int(sys.argv[1]))
request_packer = struct.Struct("4s i")

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1)
    inputs = [server]

    value = 0

    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1)
                for s in readables:
                    if s is server:
                        client, client_address = server.accept()
                        inputs.append(client)
                        print(f"Connected: {client_address[0]}:{client_address[1]}")
                        continue

                    msg = recv_exact(s, request_packer.size)
                    if not msg:
                        inputs.remove(s)
                        s.close()
                        continue

                    op, x = request_packer.unpack(msg)
                    op = op.rstrip(b"\x00")

                    match op:
                        case b"IN": value = x
                        case b"INCR": value += x
                        case b"DECR": value -= x
                        case _: 
                            inputs.remove(s)
                            s.close()
                            continue
                    reply = str(value)
                    s.sendall(struct.pack(f"{len(reply)}s", reply.encode()))
                        
            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Server stopped")