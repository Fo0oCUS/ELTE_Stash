import socket
import struct
import sys
import select

in_packer = struct.Struct("i i c")
out_packer = struct.Struct("i i")
reply_packer = struct.Struct("i")

proxy_address = ("localhost",int(sys.argv[1]))
osszeado_server_address = ("localhost",int(sys.argv[2]))
szorzo_server_address = ("localhost",int(sys.argv[3]))

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as proxy,\
    socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as osszeado_server,\
    socket.socket(socket.AF_INET, socket.SOCK_STREAM) as szorzo_server:

    szorzo_server.connect(szorzo_server_address)

    proxy.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    proxy.bind(proxy_address)
    proxy.listen(5)

    inputs = [proxy]

    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is proxy:
                client, client_address = proxy.accept()
                print("Egy kliens csatlakozott", client_address)
                inputs.append(client)
                continue

            msg = recv_exact(s,in_packer.size)

            if not msg:
                print("Kliens bontott")
                inputs.remove(s)
                s.close()
                continue

            a, b, op = in_packer.unpack(msg)

            match op:
                case b"+":
                    osszeado_server.settimeout(1.0)
                    while True:
                        osszeado_server.sendto(out_packer.pack(a,b),osszeado_server_address)
                        try:
                            data, _ = osszeado_server.recvfrom(1024)
                            break
                        except TimeoutError:
                            print("Nincs válasz, újkraküldés")
                    s.sendall(data)
                case b"*":
                    szorzo_server.sendall(out_packer.pack(a,b))
                    data = recv_exact(szorzo_server,reply_packer.size)
                    s.sendall(data)


