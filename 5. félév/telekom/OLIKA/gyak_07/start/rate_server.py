import random
import select
import socket
import struct
import sys
import time

request = struct.Struct("3s")  # devizakod, pl. b"EUR"
reply = struct.Struct("i")  # az arfolyam fillerben
rates = {}  # devizakod -> arfolyam (elso lekerdezeskor talaljuk ki)


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


server_addr = ("localhost", int(sys.argv[1]))  # pl. python rate_server.py 10001

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(server_addr)
        server.listen(5)
        print("Lassú árfolyam-szerver fut:", server_addr)
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
                    inputs.remove(s)
                    s.close()
                    continue
                (code,) = request.unpack(data)
                time.sleep(1.0)  # lassu forras: ezert erdemes gyorsitotarazni
                rate = rates.setdefault(code, random.randint(100, 99999))
                print(f"{code.decode()} -> {rate}")
                s.sendall(reply.pack(rate))
except KeyboardInterrupt:
    print("Server stopped")
