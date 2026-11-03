
import sys
import socket
import struct

packer = struct.Struct("ii1s")
server_address = ("localhost", int(sys.argv[1]))

def calculate(a, op, b):
    if op == "+":
        return a + b
    elif op == "-":
        return a - b
    elif op == "*":
        return a * b
    elif op == "/":
        return int(a / b)
    else:
        raise ValueError(f"Invalid operation: {op}")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_address)
    sock.listen(1)
    sock.settimeout(1)

    try:
        while True:
            try:
                client, client_address = sock.accept()

                with client:
                    print("Client connected:", client_address)

                    data = b""
                    while len(data) < packer.size:
                        chunk = client.recv(packer.size - len(data))
                        if not chunk:
                            break
                        data += chunk

                    if len(data) != packer.size:
                        continue

                    a, b, op = packer.unpack(data)
                    op = op.decode()

                    print(f"Clients question: {a} {op} {b}")

                    try:
                        result = calculate(a, op, b)
                        client.sendall(packer.pack(result, 0, b'R'))
                    except (ValueError, ZeroDivisionError):
                        client.sendall(packer.pack(0, 1, b'E'))

            except TimeoutError:
                pass

    except KeyboardInterrupt:
        print("Server stopped")
