import socket
import struct
import sys

packer = struct.Struct("i i c")  # ket szam es a muveleti jel
ops = {
    b"+": lambda a, b: a + b,
    b"-": lambda a, b: a - b,
    b"*": lambda a, b: a * b,
    b"/": lambda a, b: a // b,
}
server_addr = ("", int(sys.argv[1]))  # pl. python calc_server.py 10000

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_addr)
    sock.settimeout(1.0)  # Windowson a Ctrl+C csak igy szakitja meg az accept-et

    try:
        while True:
            try:
                data, client = sock.recvfrom(2048)
                print("client:", client)
                # print(f"Connected: {client[0]}:{client[1]}")
                a, b, op = packer.unpack(data)
                result = ops[op](a, b)
                print(f"{a} {op.decode()} {b} = {result}")
                sock.sendto(packer.pack(result, 0, b"R"), client)
            except TimeoutError:  # 1 mp alatt nem jott kliens: varunk tovabb
                pass
    except KeyboardInterrupt:
        print("Server stopped")
