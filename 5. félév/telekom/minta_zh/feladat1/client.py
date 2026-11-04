import sys
import socket
import struct

request_packer = struct.Struct("6s 8s 7s")
reply_packer = struct.Struct("i 10s")
final_reply_packer = struct.Struct("12s")
neptun_code = b"abcdef"
bus_code = b"LH2048"

server_address = (sys.argv[1],int(sys.argv[2]))

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
    client.connect(server_address)
    request = request_packer.pack(neptun_code,bus_code,b"request")
    client.sendall(request)

    number_of_places, travel_class = reply_packer.unpack(client.recv(reply_packer.size))

    if travel_class.strip(b"\x00") != b"economy" or number_of_places < 3:
        client.sendall(request_packer.pack(neptun_code,bus_code,b"reject"))
    else:
        client.sendall(request_packer.pack(neptun_code,bus_code,b"reserve"))

    reservation_id, = final_reply_packer.unpack(client.recv(final_reply_packer.size))
    print(reservation_id)