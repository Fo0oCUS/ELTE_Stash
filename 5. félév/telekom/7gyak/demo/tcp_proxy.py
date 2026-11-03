import select
import socket
import struct

PROXY_ADDR = ("localhost", 10000)
SERVER_ADDR = ("localhost", 10002)  # a square_server.py

in_packer = struct.Struct("10s i")  # a klienstol: nev es szam
out_packer = struct.Struct("i")  # a szervernek: csak a szam; a szerver valasza is (i)
reply_packer = struct.Struct("10s i")  # a kliensnek: a nev es a szerver valasza


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


def tcp_proxy():
    """A kliensek felé szerver (select), a szerver felé kliens: egyetlen, állandó kapcsolattal."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as backend, socket.socket(
        socket.AF_INET, socket.SOCK_STREAM
    ) as server:
        backend.connect(SERVER_ADDR)  # egyszer, a ciklus elott
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(PROXY_ADDR)
        server.listen(5)
        print("Proxy is running:", PROXY_ADDR, "->", SERVER_ADDR)
        inputs = [server]  # a backend NEM kerul ide: a valaszt rogton megvarjuk rajta
        while True:
            readables, _, _ = select.select(inputs, [], [], 1.0)
            for s in readables:
                if s is server:
                    client, addr = server.accept()
                    print(f"New client: {addr[0]}:{addr[1]}")
                    inputs.append(client)
                    continue
                msg = recv_exact(s, in_packer.size)
                if not msg:
                    inputs.remove(s)
                    s.close()
                    continue
                # atcsomagolas: a szerver csak a szamot kapja
                name, n = in_packer.unpack(msg)
                backend.sendall(out_packer.pack(n))
                (result,) = out_packer.unpack(recv_exact(backend, out_packer.size))
                shown = name.rstrip(b"\x00").decode()
                print(f"{shown}: {n} -> server -> {result}")
                s.sendall(reply_packer.pack(name, result))


if __name__ == "__main__":
    try:
        tcp_proxy()
    except KeyboardInterrupt:
        print("Proxy stopped")
