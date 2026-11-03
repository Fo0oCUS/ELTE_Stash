import socket
import struct
import time

# a kliens a proxyhoz csatlakozik, a szervert nem is ismeri
PROXY_ADDR = ("localhost", 10000)
packer = struct.Struct("10s i")  # keres: nev es szam; valasz: nev es a szam negyzete


def name_client():
    """Három kérés egy kapcsolaton, a proxyn keresztül."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect(PROXY_ADDR)
        for name, n in [(b"Anna", 7), (b"Bence", 12), (b"Csilla", -5)]:
            sock.sendall(packer.pack(name, n))
            reply_name, result = packer.unpack(sock.recv(packer.size))
            reply_name = reply_name.rstrip(
                b"\x00"
            ).decode()  # a 10s mezo vegen \x00 kitoltes
            print(f"Response: {reply_name}, {result}")
            time.sleep(1)


if __name__ == "__main__":
    name_client()
