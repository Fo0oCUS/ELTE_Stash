import socket
import struct
import sys

packer = struct.Struct("i i c")
server_address = ("localhost",int(sys.argv[1]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1.0)

    try:
        while True:
            try:
                client, client_address = server.accept()
                print(f"{client_address[0]}:{client_address[1]} has connected")

                recv = client.recv(packer.size)
                a,b,op = packer.unpack(recv)
                op = op.decode()
                print(f"Task is {a} {op} {b}")

                match op:
                    case "+": a = a + b
                    case "-": a = a - b
                    case "*": a = a * b
                    case "/": a = int(a / b) 
                b = 0
                op = "R"

                client.sendall(packer.pack(a,b,op.encode()))

                client.close()
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server closed")