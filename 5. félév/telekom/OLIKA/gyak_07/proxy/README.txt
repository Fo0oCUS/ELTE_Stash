Ide kerül a B rész proxyja megoldása, ezekkel a fájlnevekkel:

  proxy.py         indítás: python proxy.py <port> <összeadó_port> <szorzó_port>

Ellenőrzés egy paranccsal, a csomag mappájából (ahol a tests.py van):
  python tests.py check <NEPTUN> proxy

A programok minden adatot parancssori paraméterként kapnak (sys.argv), input() nélkül.
