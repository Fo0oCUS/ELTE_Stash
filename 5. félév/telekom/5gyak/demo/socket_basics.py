import socket
import struct


def wait_for_enter():
    """Megállítja a programot, amíg a felhasználó Entert nem nyom."""
    print()
    print(">>> Nyomj ENTER-t a folytatáshoz! <<<")

    input()

    print()


def section(title):
    """Új oktatási blokk kezdése."""
    print()
    print("=" * 70)
    print(title)
    print("=" * 70)
    print()


# ======================================================================
# 1. SOCKET – HOSTNÉV ÉS IP-CÍM
# ======================================================================

section("1. SOCKET – HOSTNÉV ÉS IP-CÍM")

print("socket.gethostname()")
print("---------------------")
print("A gép aktuális hostnevét adja vissza.")

hostname = socket.gethostname()

print("Hostname:", hostname)


print()
print("socket.gethostbyname()")
print("-----------------------")
print("Egy hostnévhez tartozó IPv4-címet ad vissza.")

hostip = socket.gethostbyname("www.example.org")

print("www.example.org IP-címe:", hostip)


print()
print("socket.gethostbyname_ex()")
print("--------------------------")
print("Részletesebb információkat ad vissza:")
print("- hostname")
print("- aliasok")
print("- IP-címek")

hostname, aliases, addresses = socket.gethostbyname_ex(
    "www.example.org"
)

print("Hostname:", hostname)
print("Aliasok:", aliases)
print("Címek:", addresses)


print()
print("socket.gethostbyaddr()")
print("-----------------------")
print("Egy IP-címhez próbálja meg megkeresni")
print("a hozzá tartozó hostnevet és aliasokat.")

try:
    hostname, aliases, addrs = socket.gethostbyaddr("157.181.161.79")

    print("Hostname:", hostname)
    print("Aliasok:", aliases)
    print("Címek:", addrs)

except socket.herror as e:
    print("A reverse DNS lekérdezés sikertelen.")
    print("Hiba:", e)


wait_for_enter()


# ======================================================================
# 2. PORTOK ÉS SZOLGÁLTATÁSNEVEK
# ======================================================================

section("2. AZ ELSŐ 100 PORT ÉS A getservbyport()")

print("Az első 100 TCP portot vizsgáljuk.")
print("A socket.getservbyport() megpróbálja megmondani,")
print("hogy az adott porthoz milyen ismert szolgáltatás tartozik.")
print("Csak azokat a portokat írjuk ki, amelyekhez van ismert szolgáltatás.")
print()

for port in range(1, 101):

    try:
        service = socket.getservbyport(port, "tcp")
    except OSError:
        continue                            # ehhez a porthoz nincs ismert szolgáltatás

    print(f"Port {port:3d} -> {service}")


wait_for_enter()


# ======================================================================
# 3. HTONS / NTOHS
# ======================================================================

section("3. htons() ÉS ntohs() – BYTE SORREND")

number = 543

print("Kiindulási szám:")
print(number)

print()
print("socket.htons()")
print("----------------")
print("A htons = host to network short.")
print("A host byte-sorrendből hálózati byte-sorrendre alakít.")

network_order = socket.htons(number)

print("543 hálózati byte-sorrendben:", network_order)

print()
print("socket.ntohs()")
print("----------------")
print("Az ntohs = network to host short.")
print("A hálózati byte-sorrendet visszaalakítja host sorrendre.")

host_order = socket.ntohs(network_order)

print("Visszaalakítva:", host_order)

print()
print("A két érték:")
print("Host érték:   ", number)
print("Network érték:", network_order)
print("Visszaalakítva:", host_order)

print()
print("Megjegyzés:")
print("A htons()/ntohs() a számot a gép és a hálózati")
print("byte-sorrend közötti konverzióra használja.")


wait_for_enter()


# ======================================================================
# 4. STRUCT – CSOMAGOLÁS ÉS KICSOMAGOLÁS
# ======================================================================

section("4. STRUCT – PACK ÉS UNPACK")

print("A struct modul segítségével különböző adattípusokat")
print("bináris formátumba tudunk csomagolni.")

print()
print("Formátum:")
print("i  = int")
print("2s = 2 bájtos karakterlánc")
print("f  = float")
print()

packer = struct.Struct("i 2s f")

values = (1, "ab".encode(), 2.7)

print("Eredeti értékek:")
print(values)

print()
print("pack() – bináris adat készítése:")

packed_data = packer.pack(*values)

print(packed_data)

print()
print("unpack() – bináris adat visszaalakítása:")

unpacked_data = packer.unpack(packed_data)

print(unpacked_data)

print()
print("A csomag mérete:")
print(packer.size, "bájt")

print()
print("Most network byte order (!) használatával:")

packer = struct.Struct("!i 2s f")

print("Formátum: !i 2s f")
print("A ! azt jelenti, hogy hálózati byte-sorrendet használunk.")

print()
print("A csomag mérete:")
print(packer.size, "bájt")


wait_for_enter()


# ======================================================================
# 5. STRING → BYTES → STRUCT
# ======================================================================

section("5. STRING ÉS BYTES – struct.pack()")

print("A Python szöveg (str) és a bináris adat (bytes)")
print("különböző adattípusok.")

print()

text = "hello"

print("Eredeti string:")
print(text)

print()
print("encode() után:")

encoded = text.encode()

print(encoded)

print()
print("struct.pack('8s', ...):")

packed = struct.pack("8s", encoded)

print(packed)

print()
print("A '8s' pontosan 8 bájtot jelent.")
print("A 'hello' csak 5 bájtos, ezért a maradék helyet")
print("null bájtokkal tölti ki.")

print()
print("A csomagolt adat:")
print(packed)

print()
print("decode() után:")

decoded = packed.decode()

print(repr(decoded))

print()
print("A null karakterek eltávolítása:")
print(decoded.strip("\x00"))


print()
print("=" * 70)
print("A bemutató véget ért.")
print("=" * 70)