import socket
import struct

ADDR = ("localhost", 10002)
number = struct.Struct("i")  # keres: egy szam; valasz: a negyzete


def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data


def square_server():
    """TCP szerver a proxy mögött: egy kapcsolaton (a proxyén) sok kérést szolgál ki."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(ADDR)
        server.listen(1)
        server.settimeout(1.0)  # Windowson a Ctrl+C csak igy szakitja meg az accept-et
        print("Square server is running:", ADDR)
        while True:
            try:
                conn, addr = server.accept()
            except TimeoutError:
                continue
            print(f"New connection: {addr[0]}:{addr[1]}")
            with conn:
                while data := recv_exact(conn, number.size):  # amig a proxy nem bont
                    (n,) = number.unpack(data)
                    print(f"{n} -> {n * n}")
                    conn.sendall(number.pack(n * n))
            print("The connection was closed")


if __name__ == "__main__":
    try:
        square_server()
    except KeyboardInterrupt:
        print("Server stopped")
