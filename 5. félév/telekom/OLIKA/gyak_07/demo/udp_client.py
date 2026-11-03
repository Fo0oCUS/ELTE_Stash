import socket
import time

ADDR = ("localhost", 10001)


def one_message():
    """[K1], [K2]: egy datagram a szervernek, majd várjuk a választ."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sent = sock.sendto(b"Hello server!", ADDR)
        print(f"Sent {sent} bytes, waiting for a response...")
        data, server = sock.recvfrom(1024)
        print(f"Response from {server}: {data!r}")


def three_clients():
    """[K3]: három kliens küld, mielőtt bármelyik a válaszát olvasná."""
    socks = [socket.socket(socket.AF_INET, socket.SOCK_DGRAM) for _ in range(3)]
    start = time.time()
    for i, sock in enumerate(socks, 1):
        sock.sendto(f"Hello server, {i}. kliens".encode(), ADDR)
    for i, sock in enumerate(socks, 1):
        data, _ = sock.recvfrom(1024)
        print(f"{i}. Client's response after {time.time() - start:.1f} s: {data!r}")
        sock.close()


if __name__ == "__main__":
    one_message()  # alap, [K1], [K2]: kommenteld ki, ha a [K3]-at kapcsolod be
    # three_clients()                       # [K3]
