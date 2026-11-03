import socket
import sys
import struct
#TCP Server

packer = struct.Struct("20s i")
server_address = ("localhost",int(sys.argv[1]))

with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as server:
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1.0)



    try:
        while True:
            try:
                client,client_address = server.accept()

                str,c = packer.unpack(client.recv(packer.size))

                str = str.rstrip(b"x\00").decode()
                
                client.sendall((str * c).encode())
                 
                client.close()
            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server closed")



