
import socket
import struct
import sys
import os

host = sys.argv[1]
port = int(sys.argv[2])

packer = struct.Struct("!HHI12s")
transaction_id = os.urandom(12)

request = packer.pack(0x0001, 0, 0x2112A442, transaction_id)

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
    while True:
        client.settimeout(1.0)
        client.sendto(request, (host, port))

        try:
            data, server = client.recvfrom(1024)
            break
        except socket.timeout:
            continue

    msg_type, length, cookie, received_id = packer.unpack(
        data[:packer.size]
    )

    if msg_type != 0x0101 or received_id != transaction_id:
        sys.exit("Érvénytelen STUN válasz")

    offset = packer.size

    while offset < packer.size + length:
        attr_type, attr_length = struct.unpack(
            "!HH", data[offset:offset + 4]
        )
        offset += 4

        attr_value = data[offset:offset + attr_length]

        if attr_type == 0x0020:
            zero, family, xport, xip = struct.unpack(
                "!BBHI", attr_value
            )

            public_port = xport ^ 0x2112
            public_ip = socket.inet_ntoa(
                struct.pack("!I", xip ^ 0x2112A442)
            )

            print(f"{public_ip}:{public_port}")
            break

        offset += (attr_length + 3) // 4 * 4
