
import sys
import socket
import select
import struct

server_address = ("localhost", int(sys.argv[1]))
request_packer = struct.Struct("4s i")
reply_packer = struct.Struct("i")

votes_dict = {1: 0, 2: 0, 3: 0, 4: 0, 5: 0}
user_votes = {}
buffers = {}

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(server_address)
    server.listen(5)

    inputs = [server]

    try:
        while True:
            readables, _, _ = select.select(inputs, [], [], 1.0)

            for s in readables:
                if s is server:
                    client, client_address = s.accept()
                    inputs.append(client)
                    buffers[client] = b""
                    continue

                msg = s.recv(1024)

                if not msg:
                    inputs.remove(s)
                    user_votes.pop(s, None)
                    del buffers[s]
                    s.close()
                    continue

                buffers[s] += msg

                while len(buffers[s]) >= request_packer.size:
                    packet = buffers[s][:request_packer.size]
                    buffers[s] = buffers[s][request_packer.size:]

                    parancs, szam = request_packer.unpack(packet)

                    if parancs == b"VOTE":
                        if szam < 1 or szam > 5:
                            result = -2
                        elif s in user_votes:
                            result = -1
                        else:
                            user_votes[s] = szam
                            votes_dict[szam] += 1
                            result = votes_dict[szam]

                    elif parancs == b"UNDO":
                        if szam != 0:
                            result = -2
                        elif s not in user_votes:
                            result = -1
                        else:
                            old_vote = user_votes[s]
                            votes_dict[old_vote] -= 1
                            del user_votes[s]
                            result = votes_dict[old_vote]

                    elif parancs == b"STAT":
                        if szam == 0:
                            result = sum(votes_dict.values())
                        elif 1 <= szam <= 5:
                            result = votes_dict[szam]
                        else:
                            result = -2

                    else:
                        result = -2

                    s.sendall(reply_packer.pack(result))

    except KeyboardInterrupt:
        print("Server stopped")

    finally:
        for s in inputs:
            if s is not server:
                s.close()
