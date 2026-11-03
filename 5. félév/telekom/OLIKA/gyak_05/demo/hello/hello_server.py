import socket
import sys

server_addr = ("localhost", int(sys.argv[1]))   # pl. python hello_server.py 10000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_addr)
    sock.listen(1)
    sock.settimeout(1.0)                         # Windowson a Ctrl+C csak igy szakitja meg az accept-et

    try:
        while True:
            try:
                client, client_addr = sock.accept()
                with client:
                    print("Connected:", client_addr)
                    data = client.recv(16)
                    print("Received:", data.decode())
                    client.sendall("Hello client".encode())
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")
