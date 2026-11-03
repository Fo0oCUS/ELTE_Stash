import socket
import struct
import sys
import select

sevrer_address = ("",int(sys.argv[1]))
request_packer = struct.Struct("2s i i")  # 12 byte, nem 10!
reply_packer = struct.Struct("i")

dict = {}

def recv_exact(sock, size):
    """Addig olvas, amíg pontosan size byte meg nem érkezik (vagy a másik oldal bont)."""
    data = b""
    while len(data) < size:
        chunk = sock.recv(size - len(data))
        if not chunk:
            return b""
        data += chunk
    return data

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(sevrer_address)
    server.listen(5)
    inputs = [server]

    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is server:
                client, client_address = s.accept()
                inputs.append(client)
                print("Új kliens: ", client_address)
                continue

            try:
                msg = recv_exact(s,request_packer.size)
            except ConnectionResetError:
                print("Egy klines bontott")
                inputs.remove(s)
                s.close()
                continue

            if not msg:
                print("Egy klines bontott")
                inputs.remove(s)
                s.close()
                continue

            todo,id,amount = request_packer.unpack(msg)

            if id not in dict:
                dict[id] = 0

            match todo.decode().strip("\x00"):
                case "KI": dict[id] -= amount
                case "BE": dict[id] += amount
                case _:
                    print("Egy klines bontott")
                    inputs.remove(s)
                    s.close()
                    continue 

            s.sendall(reply_packer.pack(dict[id]))



