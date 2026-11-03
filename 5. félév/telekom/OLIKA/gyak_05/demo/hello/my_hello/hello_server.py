import sys;
import socket;

server_address = ("localhost", int(sys.argv[1]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(server_address)
    sock.listen(1)
    sock.settimeout(1)

    try:
        while True:
            try:
                client, address = sock.accept()
                with client:
                    print("Client connected:", address)
                    message = client.recv(1024)
                    print("Message from clinet: " + message.decode())
                    client.sendall("Szevasz clinet szerver vagyok".encode())
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server Stopped")
