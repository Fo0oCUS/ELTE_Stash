import socket
import time

ADDR = ("localhost", 10000)


def one_message():
    with socket.create_connection(ADDR) as sock:
        sock.sendall(b"Hello server")
        print("Válasz:", sock.recv(1024))


def three_clients():
    """[K1], [K2]: három kliens csatlakozik, mielőtt bármelyik üzenne."""
    socks = []
    for i in range(1, 4):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1.0)              # [K2] ne várjunk örökké a csatlakozásra
        try:
            sock.connect(ADDR)
            sock.settimeout(None)  # a válaszra már türelmesen várunk
            print(f"{i}. kliens: csatlakozott")
            socks.append((i, sock))
        except OSError as error:
            print(f"{i}. kliens: {type(error).__name__}: {error}")
    start = time.time()
    for i, sock in socks:
        sock.sendall(b"Hello server")
    for i, sock in socks:
        try:
            print(
                f"{i}. kliens válasza {time.time() - start:.1f} s után: {sock.recv(1024)!r}"
            )
        except OSError as error:
            print(f"{i}. kliens: {type(error).__name__}: {error}")
        sock.close()


if __name__ == "__main__":
    # one_message()  # alap: kommenteld ki, ha egy kísérletet kapcsolsz be
    three_clients()                       # [K1], [K2]
