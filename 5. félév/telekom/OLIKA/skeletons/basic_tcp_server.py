import sys
import socket
import struct

packer = struct.Struct("ii1s")
server_address = ("localhost", int(sys.argv[1]))

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

    try:
        while True:
            try:
                client, client_address = server.accept()

                with client:
                    data = recv_exact(client, data.size)
                    client.sendall(data_to_send)

            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")