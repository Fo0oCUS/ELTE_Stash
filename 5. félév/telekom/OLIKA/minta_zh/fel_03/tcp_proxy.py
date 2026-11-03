import sys
import socket
import select
import struct


proxy_address = ("localhost", int(sys.argv[1]))
udp_server_address = ("localhost", int(sys.argv[2]))

client_in_packer = struct.Struct("5s i")
server_in_packer = struct.Struct("5s i")
server_out_packer = struct.Struct("i")

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as udp_server, \
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

                    udp_server.sendto(msg, udp_server_address)
                    data, _ = udp_server.recvfrom(1024)
                    print(server_out_packer.unpack(data)[0])
                    s.sendall(data)

            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Proxy server stopped!")
