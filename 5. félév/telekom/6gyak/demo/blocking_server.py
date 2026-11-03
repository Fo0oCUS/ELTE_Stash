import socket
import time

ADDR = ("localhost", 10000)


def blocking_server():
    """Egyszerre egy klienssel foglalkozik: accept -> recv -> válasz -> bontás."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)   # [K3] második rész
        server.bind(ADDR)
        server.listen(2)  # [K1] a lehető legrövidebb várakozási sor
        print("Blokkoló szerver fut:", ADDR)
        while True:
            client, addr = server.accept()
            with client:
                msg = client.recv(1024)
                print(f"{addr}: {msg!r}")
                time.sleep(2)  # [K1] lassú kiszolgálás, hogy lássuk a sorban állást
                client.sendall(b"Hello client")
                client.recv(
                    1024
                )  # megvárjuk, hogy a kliens bontson: így a port újraindításkor szabad


if __name__ == "__main__":
    blocking_server()  # [K1]-[K3]
