import socket
import struct
import sys
import select
import time

packer = struct.Struct("i i c")  # ket szam es a muveleti jel
ops = {
    b"+": lambda a, b: a + b,
    b"-": lambda a, b: a - b,
    b"*": lambda a, b: a * b,
    b"/": lambda a, b: a // b,
}
server_addr = ("", int(sys.argv[1]))  # pl. python calc_server.py 10000

def recv_exact(sock, size):
    """Addig olvas, amíg pontosan size byte meg nem érkezik (vagy a másik oldal bont)."""
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_addr)
    sock.listen(5)

    inputs = [sock]
    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is sock:
                client, addr = sock.accept()
                inputs.append(client)
                print("Új kliens:", addr)
                continue
            
            msg = recv_exact(s,packer.size)

            if not msg:
                print("Egy kliens bontott")
                s.close()
                inputs.remove(s)
                continue

            a, b, op = packer.unpack(msg)
            result = ops[op](a, b)
            print(f"{a} {op.decode()} {b} = {result}")
            s.sendall(packer.pack(result, 0, b"R"))

