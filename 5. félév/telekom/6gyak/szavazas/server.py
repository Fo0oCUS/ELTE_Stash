import socket
import select
import sys
import struct

server_address = ("",int(sys.argv[1]))
appointments = {
    1:0,
    2:0,
    3:0,
    4:0,
    5:0
}
votes = {}

request_packer = struct.Struct("4s i")    # 8 byte
reply_packer = struct.Struct("i")

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
    server.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    server.bind(server_address)
    server.listen(5)

    inputs = [server]    

    while True:
        readables, _, _ = select.select(inputs, [], [], 1.0)
        for s in readables:
            if s is server:
                print("Kliens csatlakozott")
                client, client_address = s.accept()
                inputs.append(client)
                continue

            msg = recv_exact(s,request_packer.size)

            if not msg:
                print("Egy kliens bontott")
                inputs.remove(s)
                s.close()
                votes.pop(s,None)
                continue

            command, n = request_packer.unpack(msg)

                
            match command:
                case b"VOTE":
                    if s in votes:
                        s.sendall(reply_packer.pack(-1))
                        continue

                    if n not in appointments:
                        s.sendall(reply_packer.pack(-2))
                        continue

                    votes[s] = n
                    appointments[n] += 1
                    s.sendall(reply_packer.pack(appointments[n]))
                case b"UNDO":
                    if s not in votes:
                        s.sendall(reply_packer.pack(-1))
                        continue

                    if n != 0:
                        s.sendall(reply_packer.pack(-2))
                        continue

                    n = votes[s]
                    appointments[n] -= 1
                    votes.pop(s)
                    s.sendall(reply_packer.pack(appointments[n]))
                case b"STAT":
                    if n == 0:
                        s.sendall(reply_packer.pack(sum(appointments.values())))
                        continue

                    if n not in appointments:
                        s.sendall(reply_packer.pack(-2))
                        continue

                    s.sendall(reply_packer.pack(appointments[n]))
                case _:
                    s.sendall(reply_packer.pack(-2))

                    



            





