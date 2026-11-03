import socket
import struct
import sys
import random
import time

packer = struct.Struct("i i c")  # ket szam es a muveleti jel
# pl. python calc_client.py localhost 10000 6 x 7   (a szorzas jele itt x, mert a * a shellben mast jelent)
# host, port, a, op, b = (
#     sys.argv[1],
#     int(sys.argv[2]),
#     int(sys.argv[3]),
#     sys.argv[4],
#     int(sys.argv[5]),
# )
# op = "*" if op == "x" else op
host, port = (
    sys.argv[1],
    int(sys.argv[2])
)

socks = []
for i in range(1,4):
    sock = socket.create_connection((host, port))
    print(f"{i}. kliens: csatlakozott")
    socks.append(sock)

for sock in socks:
    for i in range(1,6):
        a, b = random.randint(1, 10), random.randint(1, 10)
        op = random.choice(["+", "-", "*", "/"])
        sock.sendall(packer.pack(a, b, op.encode()))
        result, _, _ = packer.unpack(sock.recv(packer.size))
        print(f"{a} {op} {b} = {result}")
        time.sleep(2)

for sock in socks:
    sock.close()

# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
#     client.connect((host, port))
#     client.sendall(packer.pack(a, b, op.encode()))
#     result, _, _ = packer.unpack(client.recv(packer.size))
#     print(f"{a} {op} {b} = {result}")
