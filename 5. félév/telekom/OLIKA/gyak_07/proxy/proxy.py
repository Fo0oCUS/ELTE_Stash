import sys
import socket
import select
import struct


proxy_address = ("localhost", int(sys.argv[1]))
add_server_address = ("localhost", int(sys.argv[2]))
mult_server_address = ("localhost", int(sys.argv[3]))

client_in_packer = struct.Struct("i i c")
server_out_packer = struct.Struct("i i")
server_in_packer = struct.Struct("i")

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as add_server, \
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as mult_server, \
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy_server:

    mult_server.connect(mult_server_address)

    proxy_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy_server.bind(proxy_address)
    proxy_server.listen(5)
    proxy_server.settimeout(1)

    inputs = [proxy_server]
    buffers = {}

    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1.0)
                for s in readables:
                    if s is proxy_server:
                        client, client_address = s.accept()
                        buffers[client] = b""
                        inputs.append(client)
                        continue
    
                    msg = s.recv(client_in_packer.size)
                    if not msg:
                        del buffers[s]
                        inputs.remove(s)
                        s.close()
                        continue

                    buffers[s] += msg
                    while len(buffers[s]) >= client_in_packer.size:
                        packet = buffers[s][:client_in_packer.size]
                        buffers[s] = buffers[s][client_in_packer.size:]

                        a, b, op = client_in_packer.unpack(packet)
                        data_to_servers = server_out_packer.pack(a, b)
                        if op == b"+":
                            add_server.settimeout(1.0)

                            while True:
                                add_server.sendto(data_to_servers, add_server_address)

                                try:
                                    data, _ = add_server.recvfrom(server_in_packer.size)
                                    break
                                except TimeoutError:
                                    print("Nincs valasz, ujrakuldes!")

                            s.sendall(data)
                        elif op == b"*":
                            mult_server.sendall(data_to_servers)
                            data = recv_exact(mult_server, server_in_packer.size)
                            s.sendall(data)
                        else:
                            del buffers[s]
                            inputs.remove(s)
                            s.close()
                            break

            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Proxy server stopped!")
