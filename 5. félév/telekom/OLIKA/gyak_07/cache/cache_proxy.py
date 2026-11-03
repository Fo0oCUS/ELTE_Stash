import sys
import struct
import select
import socket

proxy_address = ("localhost", int(sys.argv[1]))
server_address = ("localhost", int(sys.argv[2]))

request_packer = struct.Struct("3s")
reply_packer = struct.Struct("i")

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server, \
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy:

    server.connect(server_address)

    proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy.bind(proxy_address)
    proxy.listen(1)
    proxy.settimeout(1.0)

    inputs = [proxy]
    cache = {}

    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1.0)
                for s in readables:
                    if s is proxy:
                        client, address = s.accept()
                        inputs.append(client)
                        continue

                    msg = recv_exact(s, request_packer.size)
                    if not msg:
                        inputs.remove(s)
                        s.close()
                        continue

                    dev, = request_packer.unpack(msg)
                    if dev not in cache:
                        server.sendall(msg)
                        data, = reply_packer.unpack(recv_exact(server, reply_packer.size))
                        cache[dev] = data

                    s.sendall(reply_packer.pack(cache[dev]))    

            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Proxy server stopped")
     