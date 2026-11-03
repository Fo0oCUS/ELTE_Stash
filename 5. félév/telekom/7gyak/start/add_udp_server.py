import socket
import struct
import sys

request = struct.Struct("i i")  # ket szam
reply = struct.Struct("i")  # az osszeguk
server_addr = ("localhost", int(sys.argv[1]))  # pl. python add_udp_server.py 10001

try:
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(server_addr)
        sock.settimeout(1.0)  # Windowson a Ctrl+C csak igy szakitja meg a recvfrom-ot
        print("Összeadó szerver fut (UDP):", server_addr)
        while True:
            try:
                data, client = sock.recvfrom(1024)
            except TimeoutError:
                continue
            if len(data) != request.size:
                print(f"{client}: {len(data)} byte jött, {request.size} kellene")
                continue
            a, b = request.unpack(data)
            print(f"{client}: {a} + {b} = {a + b}")
            sock.sendto(reply.pack(a + b), client)
except KeyboardInterrupt:
    print("Server stopped")
