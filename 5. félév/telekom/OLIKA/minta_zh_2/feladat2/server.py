import sys
import socket
import struct
import select
# from pathlib import Path

server_in_packer = struct.Struct("3s 6s 8s i")
server_out_packer = struct.Struct("i")
server_address = ("localhost", int(sys.argv[1]))

def recv_exact(sock, size):
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

database = {}

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(1)
    server.settimeout(1)

    inputs = [server]
    try:
        while True:
            try:
                readables, _, _ = select.select(inputs, [], [], 1.0)
                for s in readables:
                    if s is server:
                        client, client_address = s.accept()
                        inputs.append(client)
                        continue

                    msg = recv_exact(s, server_in_packer.size)
                    if not msg:
                        inputs.remove(s)
                        s.close()
                        continue

                    op, raktar, termek, db = server_in_packer.unpack(msg)
                    op, raktar, termek = op.decode(), raktar.rstrip(b"\x00").decode(), termek.rstrip(b"\x00").decode()
                    if raktar not in database:
                        database[raktar] = []
                    
                    # with open(Path(__file__).parent / "log.txt", "w") as f:
                    #     f.write(op)
                    match op:
                        case "ADD":
                            found = False
                            for tuple in database[raktar]:
                                if tuple[0] == termek:
                                    tuple[1] += db
                                    found = True
                                    s.sendall(server_out_packer.pack(tuple[1]))
                                    break
                            if not found:
                                database[raktar].append((termek, db))
                                s.sendall(server_out_packer.pack(db))
                        case "GET":
                            found = False
                            for tuple in database[raktar]:
                                if tuple[0] == termek:
                                    found = True
                                    s.sendall(server_out_packer.pack(tuple[1]))
                                    break
                            if not found:
                                s.sendall(server_out_packer.pack(0))
                        case "TOT":
                            sum = 0
                            for tuple in database[raktar]:
                                sum += tuple[1]
                            s.send(server_out_packer.pack(sum))
                        case _:
                            inputs.remove(s)
                            s.close()
                            continue


            except TimeoutError:
                pass
    except KeyboardInterrupt:
        print("Server stopped")