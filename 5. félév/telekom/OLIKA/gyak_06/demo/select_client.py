import socket
import struct
import time

ADDR = ("localhost", 10000)
packer = struct.Struct(
    "i i c"
)  # a [K7] kísérlet üzenete: két szám és egy műveleti jel, 9 byte


def one_message():
    with socket.create_connection(ADDR) as sock:
        sock.sendall(b"Hello server")
        print("Válasz:", sock.recv(1024))


def three_clients():
    """[K4]: három kliens csatlakozik, mielőtt bármelyik üzenne."""
    socks = []
    for i in range(1, 4):
        sock = socket.create_connection(ADDR)
        print(f"{i}. kliens: csatlakozott")
        socks.append((i, sock))
    start = time.time()
    for i, sock in socks:
        sock.sendall(b"Hello server")
    for i, sock in socks:
        print(f"{i}. kliens válasza {time.time() - start:.1f} s után: {sock.recv(1024)!r}")
        sock.close()


def two_messages():
    """[K6]: két üzenet gyorsan egymás után."""
    with socket.create_connection(ADDR) as sock:
        sock.sendall(b"Hello server")
        sock.sendall(b"Hello server")
        time.sleep(2)
        print("Válasz:", sock.recv(1024))


def split_message():
    """[K7]: egy 9 byte-os üzenet két részletben, köztük szünettel."""
    msg = packer.pack(12, 5, b"+")
    with socket.create_connection(ADDR) as sock:
        sock.sendall(msg[:4])
        print("Elküldve az első 4 byte, várok...")
        time.sleep(1)
        sock.sendall(msg[4:])
        print("Elküldve a maradék 5 byte")
        time.sleep(1)
        print("Válasz:", sock.recv(1024))


if __name__ == "__main__":
    # one_message()  # alap: kommenteld ki, ha egy kísérletet kapcsolsz be
    # three_clients()                       # [K4]
    # two_messages()                        # [K6]
    split_message()                       # [K7]
