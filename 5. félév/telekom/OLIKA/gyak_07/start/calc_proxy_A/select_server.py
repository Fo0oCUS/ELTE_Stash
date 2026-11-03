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


proxy_address = ("localhost", int(sys.argv[1]))  # pl. python select_server.py 10000
server_address = ("localhost", int(sys.argv[2]))

try:
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server, \
         socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy:

        proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        proxy.bind(proxy_address)
        proxy.listen(5)
        server.connect(server_address)
        print("Proxy server running:", proxy_address)
        inputs = [proxy]
        while True:
            # a select megmondja, melyik socketen van teendo (1 mp utan mindenkepp visszater)
            readables, _, _ = select.select(inputs, [], [], 1.0)
            for s in readables:
                if s is proxy:  # uj kliens csatlakozik
                    client, client_address = proxy.accept()
                    print(f"New client: {client_address[0]}:{client_address[1]}")
                    inputs.append(client)
                    continue
                msg = recv_exact(s, packer.size)  # a teljes 9 byte-ot megvarjuk
                if not msg:  # b"" -> a kliens bontotta a kapcsolatot
                    print(f"{s.getpeername()}: closed the connection")
                    inputs.remove(s)
                    s.close()
                    continue
                print(f"{s.getpeername()}: {packer.unpack(msg)}")
                server.sendto(msg, server_address)
                data, server_adress = server.recvfrom(packer.size)
                s.sendall(data)
except KeyboardInterrupt:
    print("Server stopped")
