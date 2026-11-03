import socket
import struct
import sys

tcp_server_addr = (sys.argv[1], int(sys.argv[2]))
udp_server_addr = (sys.argv[1], int(sys.argv[3]))
tcp_packer = struct.Struct("4s i")
udp_packer = struct.Struct("5s i")
tcp_op, tcp_x = sys.argv[4], int(sys.argv[6])
udp_op, udp_x = sys.argv[5], int(sys.argv[6])

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

for i in range(1000):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as tcp_server, \
        socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as udp_server:
        tcp_server.connect(tcp_server_addr)
        tcp_server.sendall(tcp_packer.pack(tcp_op.encode(), tcp_x))
        tcp_data = int(tcp_server.recv(16).decode())
        print("Received from TCP:", tcp_data)

        udp_server.sendto(udp_packer.pack(udp_op.encode(), udp_x), udp_server_addr)
        udp_data, _ = udp_server.recvfrom(1024)
        udp_data = int(struct.unpack("i", udp_data)[0])
        print("Received from UDP:", udp_data)

        print(f"Difference: {udp_data - tcp_data}")