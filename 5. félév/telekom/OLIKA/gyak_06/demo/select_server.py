import select
import socket
import struct
import time

ADDR = ("localhost", 10000)
packer = struct.Struct(
    "i i c"
)  # a [K7] kísérlet üzenete: két szám és egy műveleti jel, 9 byte


def recv_exact(sock, size):
    """Addig olvas, amíg pontosan size byte meg nem érkezik (vagy a másik oldal bont)."""
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


def select_server():
    """Egyszerre több klienssel foglalkozik: a select megmondja, melyik socketen van teendő."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind(ADDR)
        server.listen(5)
        print("Select szerver fut:", ADDR)
        inputs = [server]
        while True:
            readables, _, _ = select.select(inputs, [], [], 1.0)
            for s in readables:
                if s is server:  # új kliens csatlakozik
                    client, addr = server.accept()
                    inputs.append(client)
                    print("Új kliens:", addr)
                    continue
                time.sleep(1)                             # [K6] a szerver lassú: vár egy kicsit olvasás előtt
                # msg = s.recv(1024)  # [K7] javításnál ezt kommenteld ki...
                msg = recv_exact(s, packer.size)          # [K7] ...és ezt kapcsold be: a teljes 9 byte-ot várjuk meg
                if not msg:  # b"" -> a kliens bontotta a kapcsolatot
                    print("Egy kliens bontott")
                    s.close()
                    inputs.remove(s)  # [K5] tedd kommentbe ezt a sort!
                    continue
                print(f"{s.getpeername()}: {msg!r} ({len(msg)} byte)")
                print("Kicsomagolva:", packer.unpack(msg))  # [K7] a kapott byte-ok mint (i i c)
                s.sendall(b"OK")


if __name__ == "__main__":
    select_server()  # [K4]-[K7]
