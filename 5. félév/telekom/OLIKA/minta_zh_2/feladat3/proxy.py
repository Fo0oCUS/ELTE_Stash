import sys
import socket
import select
import struct


proxy_address = ("localhost", int(sys.argv[1]))
server_address = ("localhost", int(sys.argv[2]))

client_in_packer = struct.Struct("i 20s")
server_in_packer = struct.Struct("5s 20s")

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server, \
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy_server:

    proxy_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy_server.bind(proxy_address)
    proxy_server.listen(5)
    proxy_server.settimeout(1)

    inputs = [proxy_server]

    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1.0)
                for s in readables:
                    if s is proxy_server:
                        client, client_address = s.accept()
                        inputs.append(client)
                        continue
    
                    msg = recv_exact(s, client_in_packer.size)
                    if not msg:
                        inputs.remove(s)
                        s.close()
                        continue

                    tipus, username = client_in_packer.unpack(msg)

                    match tipus:
                        case 0:
                            server.sendto(server_in_packer.pack(b"LOGIN", username), server_address)
                        case 1:
                            server.sendto(server_in_packer.pack(b"GUEST", username), server_address)
                            
                    data, _ = server.recvfrom(4096)
                    s.sendall(data)

            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Proxy server stopped!")
