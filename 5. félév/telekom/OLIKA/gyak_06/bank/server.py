import sys
import socket
import select
import struct

server_address = ("localhost", int(sys.argv[1]))
request_packer = struct.Struct("2s i i")
reply_packer = struct.Struct("i")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1)
    inputs = [server]
    data = []
    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1)
                for s in readables:
                    if s is server:
                        client, client_address = server.accept()
                        inputs.append(client)
                        print(f"Connected: {client_address[0]}:{client_address[1]}")
                        continue

                    msg = s.recv(request_packer.size)
                    if not msg:
                        inputs.remove(s)
                        s.close()
                        continue

                    try:
                        parancs, azon, osszeg = request_packer.unpack(msg)
                        if parancs != b"KI" and parancs != b"BE":
                            raise Exception
                    except Exception:
                        inputs.remove(s)
                        s.close()
                        continue

                    user_data = None
                    for row in data:
                        if row[0] == azon:
                            user_data = row

                    if user_data is None:
                        user_data = [azon, 0]
                        data.append(user_data)

                    if parancs.decode() == "BE":
                        user_data[1] += osszeg
                    elif parancs.decode() == "KI":
                        user_data[1] -= osszeg

                    s.sendall(reply_packer.pack(user_data[1]))
                        
            except TimeoutError:
                continue
    except KeyboardInterrupt:
        print("Server stopped")
