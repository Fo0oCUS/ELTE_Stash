import socket
import struct
import sys

packer = struct.Struct("i i c")  # ket szam es a muveleti jel
# pl. python calc_client.py localhost 10000 6 x 7   (a szorzas jele itt x, mert a * a shellben mast jelent)
host, port, a, op, b = (
    sys.argv[1],
    int(sys.argv[2]),
    int(sys.argv[3]),
    sys.argv[4],
    int(sys.argv[5]),
)
op = "*" if op == "x" else op

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
    client.sendto(packer.pack(a, b, op.encode()),(host, port))
    data, server = client.recvfrom(1024)
    result, _, _ = packer.unpack(data)
    print(f"{a} {op} {b} = {result}")
