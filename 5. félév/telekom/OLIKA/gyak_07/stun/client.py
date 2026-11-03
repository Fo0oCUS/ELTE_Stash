import os
import sys
import socket
import struct
import time

server_address = (sys.argv[1], int(sys.argv[2]))
packer = struct.Struct("!HHI12s")

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
    client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    client.settimeout(1.0)

    transaction_id = os.urandom(12)

    data_to_server = packer.pack(0x0001, 0, 0x2112A442, transaction_id)

    while True:
        try:
            client.sendto(data_to_server, server_address)
            data, address = client.recvfrom(4096)

            msg_type, length, cookie, received_id = packer.unpack(data[:20])

            if msg_type != 0x0101 or cookie != 0x2112A442 or received_id != transaction_id:
                continue

            offset = 20
            while offset < 20 + length:
                typ, size = struct.unpack("!HH", data[offset:offset+4])

                if typ == 0x0020:
                    _, _, xport, xip = struct.unpack("!BBHI", data[offset+4:offset+12])
                    ip = socket.inet_ntoa(struct.pack("!I", xip ^ 0x2112A442))
                    print(f"{ip}:{xport ^ 0x2112}")
                    break

                offset += 4 + (size + 3) // 4 * 4

            if typ == 0x0020:
                break  # Kilép a külső while True-ból is

        except TimeoutError:
            continue
    
