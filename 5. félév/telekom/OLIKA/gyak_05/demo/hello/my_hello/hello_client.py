import sys
import socket

server_address = (sys.argv[1], int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    print("Clinet connected to server at: ", server_address)
    client.sendall("Szia szerver clinet vagyok".encode())
    message_from_server = client.recv(1024)
    print("Szerver küldte: " + message_from_server.decode())

