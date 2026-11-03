import socket

ADDR = ("localhost", 10001)


def hello_server():
    """UDP szerver: nincs listen, accept; minden datagram a feladó címével együtt érkezik."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(ADDR)
        print("UDP server is running:", ADDR)
        while True:
            data, client = sock.recvfrom(1024)
            # data, client = sock.recvfrom(5)  # [K2] kisebb puffer, mint az üzenet
            print(f"{client}: {data!r} ({len(data)} byte)")
            sock.sendto(b"Hello client", client)


if __name__ == "__main__":
    hello_server()  # [K1]-[K3]
