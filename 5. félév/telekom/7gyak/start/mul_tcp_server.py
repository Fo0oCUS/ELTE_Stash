import select
import socket
import struct
import sys

request = struct.Struct("i i")  # ket szam
reply = struct.Struct("i")  # a szorzatuk


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


server_addr = ("localhost", int(sys.argv[1]))  # pl. python mul_tcp_server.py 10002

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(server_addr)
        server.listen(5)
        print("Szorzó szerver fut (TCP):", server_addr)
        inputs = [server]
        while True:
            readables, _, _ = select.select(inputs, [], [], 1.0)
            for s in readables:
                if s is server:
                    client, addr = server.accept()
                    print(f"New client: {addr[0]}:{addr[1]}")
                    inputs.append(client)
                    continue
                data = recv_exact(s, request.size)
                if not data:
                    print(f"{s.getpeername()}: closed the connection")
                    inputs.remove(s)
                    s.close()
                    continue
                a, b = request.unpack(data)
                print(f"{s.getpeername()}: {a} * {b} = {a * b}")
                s.sendall(reply.pack(a * b))
except KeyboardInterrupt:
    print("Server stopped")
