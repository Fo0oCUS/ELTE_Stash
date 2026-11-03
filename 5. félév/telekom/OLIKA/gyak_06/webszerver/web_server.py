import sys
import socket
import select
from pathlib import Path
import mimetypes

server_address = (sys.argv[1], int(sys.argv[2]))
www_folder_name = sys.argv[3]

buffers = {}

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1)

    inputs = [server]
    www = Path(www_folder_name).resolve()

    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1.0)
                for s in readables:
                    if s is server:
                        client, client_address = s.accept()
                        inputs.append(client)
                        buffers[client] = b""
                        continue

                    buffers[s] += s.recv(16)
                    if b"\r\n\r\n" not in buffers[s]:
                        continue

                    request = buffers[s].decode()
                    filename = request.split(" ")[1].lstrip("/")
                    path = (www / filename).resolve()

                    if path.is_relative_to(www) and path.is_file():
                        # print("200 OK")
                        body = path.read_bytes()

                        response = "HTTP/1.1 200 OK\r\n"
                        response += f"Content-Type: {mimetypes.guess_type(filename)[0] or 'application/octet-stream'}\r\n"
                        response += f"Content-Length: {len(body)}\r\n"
                        response += "Connection: close\r\n"
                        response += "\r\n"

                        s.sendall(response.encode() + body)
                    else:
                        # print("404 Not Found")
                        body = b"Not Found"

                        response = "HTTP/1.1 404 Not Found\r\n"
                        response += "Content-Type: text/plain; charset=utf-8\r\n"
                        response += f"Content-Length: {len(body)}\r\n"
                        response += "Connection: close\r\n"
                        response += "\r\n"

                        s.sendall(response.encode() + body)

                    del buffers[s]
                    inputs.remove(s)
                    s.close()
            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Server stopped")
    