import select
import socket
import struct
import sys

packer = struct.Struct("i i c")  # ket szam es a muveleti jel, 9 byte


def recv_exact(sock, size):
    """Addig olvas, amig pontosan size byte meg nem erkezik (vagy a masik oldal bont)."""
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


server_addr = ("localhost", int(sys.argv[1]))  # pl. python select_server.py 10000

try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(server_addr)
        server.listen(5)
        print("Select server running:", server_addr)
        inputs = [server]
        while True:
            # a select megmondja, melyik socketen van teendo (1 mp utan mindenkepp visszater)
            readables, _, _ = select.select(inputs, [], [], 1.0)
            for s in readables:
                if s is server:  # uj kliens csatlakozik
                    client, addr = server.accept()
                    print(f"New client: {addr[0]}:{addr[1]}")
                    inputs.append(client)
                    continue
                msg = recv_exact(s, packer.size)  # a teljes 9 byte-ot megvarjuk
                if not msg:  # b"" -> a kliens bontotta a kapcsolatot
                    print(f"{s.getpeername()}: closed the connection")
                    inputs.remove(s)
                    s.close()
                    continue
                print(f"{s.getpeername()}: {packer.unpack(msg)}")
                s.sendall(b"OK")
except KeyboardInterrupt:
    print("Server stopped")
