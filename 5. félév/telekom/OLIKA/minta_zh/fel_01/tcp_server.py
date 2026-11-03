import sys
import socket
import struct

request_packer = struct.Struct("20s i")
server_address = ("localhost", int(sys.argv[1]))

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

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
                    data = recv_exact(client, request_packer.size)
                    text, num = request_packer.unpack(data)
                    new = text.decode()[:num][::-1]
                    client.sendall(struct.pack(f"{num}s", new.encode()))
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")