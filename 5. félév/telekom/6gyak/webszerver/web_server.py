import select
import sys
import socket
import mimetypes
import os

server_address = (sys.argv[1], int(sys.argv[2]))
www = sys.argv[3]

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(5)

    inputs = [server]
    buffers = {}

    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)

        for s in readables:

            if s is server:
                client, client_address = server.accept()
                inputs.append(client)
                buffers[client] = b""
                continue

            chunk = s.recv(4096)

            if not chunk:
                inputs.remove(s)
                buffers.pop(s, None)
                s.close()
                continue

            buffers[s] += chunk

            if b"\r\n\r\n" not in buffers[s]:
                continue

            msg = buffers.pop(s)

            file_name = msg.split(b"\r\n")[0].split()[1].decode().lstrip("/")
            path = os.path.join(www, file_name)

            if os.path.exists(path):
                with open(path, "rb") as f:
                    content = f.read()

                response = (
                    f"HTTP/1.1 200 OK\r\n"
                    f"Content-Type: {mimetypes.guess_type(file_name)[0]}\r\n"
                    f"Content-Length: {len(content)}\r\n"
                    f"Connection: close\r\n"
                    f"\r\n"
                ).encode() + content

            else:
                content = b"Not Found"

                response = (
                    f"HTTP/1.1 404 Not Found\r\n"
                    f"Content-Type: text/plain; charset=utf-8\r\n"
                    f"Content-Length: {len(content)}\r\n"
                    f"Connection: close\r\n"
                    f"\r\n"
                ).encode() + content

            s.sendall(response)
            inputs.remove(s)
            s.close()