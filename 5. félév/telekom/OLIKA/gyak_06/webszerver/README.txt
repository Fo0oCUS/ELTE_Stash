Ide kerül a saját webszerver megoldása, ezekkel a fájlnevekkel:

  web_server.py  indítás: python web_server.py <host> <port> www   (a mellette lévő www mappát szolgálja ki)

Ellenőrzés egy paranccsal, a csomag mappájából (ahol a tests.py van):
  python tests.py check <NEPTUN> webszerver

A programok minden adatot parancssori paraméterként kapnak (sys.argv), input() nélkül.
