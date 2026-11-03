import socket
import ssl

host = "www.example.org"
request = f"GET / HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n"


def http_plain():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, 80))
    sock.sendall(request.encode())
    print(sock.recv(200).decode())
    sock.close()


def https_tls():
    context = ssl.create_default_context()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tls = context.wrap_socket(sock, server_hostname=host)
    tls.connect((host, 443))  # TCP kapcsolat + TLS kézfogás
    print(tls.version())  # pl. TLSv1.3
    tls.sendall(request.encode())
    print(tls.recv(200).decode())
    tls.close()


def http_to_https():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, 443))
    sock.sendall(request.encode())
    print(sock.recv(300).decode(errors="replace"))
    sock.close()


if __name__ == "__main__":
    for title, step in [
        ("1. HTTP a 80-as porton", http_plain),
        ("2. HTTPS (TLS) a 443-as porton", https_tls),
        ("3. HTTP TLS nélkül a 443-as portra", http_to_https),
    ]:
        print("=" * 60)
        print(title)
        print("=" * 60)
        step()
        input("\n>>> Nyomj ENTER-t a folytatáshoz! <<<\n")
