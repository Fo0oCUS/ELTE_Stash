"""Tesztprogramok indítása a Telekommunikációs Hálózatok gyakorlathoz.

A tesztek egy Docker konténerben futnak, ehhez csak a Docker Desktopnak kell futnia.
A Dockerhez nem kell értened, mindent ez a script intéz.

  python tests.py check <NEPTUN> <feladat>       EGY parancs: elindítja a szerveredet és/vagy a kliensedet,
                                                lefuttat minden tesztet, és kiírja az összes kódot.
                                                A programjaidat a feladat mappájában keresi (lásd lent).

Kézi mód, lépésenkénti fejlesztéshez:
  python tests.py start <NEPTUN> [feladat ...]  elindítja a teszteket, és ebben az ablakban mutatja
                                                a tesztszerverek naplóját; Ctrl+C: a tesztek leállnak.
                                                Ha nem adsz meg feladatot, minden tesztszerver elindul.
  python tests.py test <feladat> <port>          (egy MÁSIK ablakban) a feladat tesztkliense a te
                                                szerveredet teszteli
  python tests.py log                            a napló újra megnyitása, ha bezártad (kilépés: Ctrl+C)
  python tests.py stop                           leállítja a teszteket (ha az ablakot Ctrl+C nélkül zártad be)

Feladatok és a programjaid helye (a tests.py mellett):
  szamologep   szamologep/server.py, szamologep/client.py
  barkoba      barkoba/client.py
  dns          dns/client.py
  szavazas     szavazas/server.py
  bank         bank/server.py
  webszerver   webszerver/web_server.py (a webszerver/www mappát szolgálja ki)
  proxy        proxy/proxy.py
  cache        cache/cache_proxy.py
  stun         stun/client.py
Más fájlnév esetén: python tests.py check <NEPTUN> <feladat> --server <fájl> --client <fájl>

Példák:
  python tests.py check ABC123 szamologep        a számológép szerverét és kliensét is leteszteli
  python tests.py start ABC123 barkoba           csak a barkóba tesztszervere indul el
  python tests.py test szamologep 10000          a 10000-es porton futó szerveredet teszteli
"""

import os
import re
import shutil
import signal
import socket
import subprocess
import sys
import time

IMAGE = "python:3.12-slim"       # the obfuscated tests only run with exactly this Python version
CONTAINER = "telkom-tesztek"
PORTS = "20001-20010"            # the test servers listen here
HERE = os.path.dirname(os.path.abspath(__file__))
TESTS_DIR = os.path.join(HERE, "tesztek")

# what "check" runs for each task; paths are relative to this folder, {port}/{neptun} are filled in
CHECKS = {
    "szamologep": {"server": "szamologep/server.py", "client": "szamologep/client.py", "port": 20001,
                   "runs": [["12", "+", "5"], ["20", "-", "7"], ["6", "*", "7"], ["-9", "/", "2"]]},
    "barkoba": {"client": "barkoba/client.py", "port": 20002, "runs": [["{neptun}"]]},
    "dns": {"client": "dns/client.py", "port": 20003, "runs": [["{neptun}.telkom"]]},
    "szavazas": {"server": "szavazas/server.py"},
    "bank": {"server": "bank/server.py"},
    "webszerver": {"server": "webszerver/web_server.py", "server_args": ["localhost", "{port}", "www"]},
    "proxy": {"server": "proxy/proxy.py", "server_args": ["{port}", "20004", "20005"],
              "ready": ["proxy összeadó szerver", "proxy szorzó szerver"]},
    "cache": {"server": "cache/cache_proxy.py", "server_args": ["{port}", "20006"], "ready": ["cache háttérszerver"]},
    "stun": {"client": "stun/client.py", "port": 20007, "runs": [[]]},
    "rdt": {"client": "rdt/rdt_sender.py", "port": 20008, "runs": [["kep.png"]], "timeout": 90},
}
SERVER_START = 5       # seconds the student's server has to start listening
CLIENT_RUN = 10        # seconds one run of the student's client may take
CHILD_ENV = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUNBUFFERED="1")


for stream in (sys.stdout, sys.stderr):
    stream.reconfigure(errors="replace")      # consoles that cannot show Hungarian letters


def docker(*args, capture=False):
    return subprocess.run(["docker", *args], text=True, encoding="utf-8", errors="replace",
                          capture_output=capture)


def stream_docker(*args):
    """Runs docker and prints its output line by line (the container writes UTF-8)."""
    process = subprocess.Popen(["docker", *args], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                               text=True, encoding="utf-8", errors="replace")
    try:
        for line in process.stdout:
            print(line, end="", flush=True)
    except KeyboardInterrupt:
        process.terminate()
    process.wait()


def check_docker():
    if shutil.which("docker") is None:
        sys.exit("Nem találom a Dockert. Telepítsd a Docker Desktopot: https://www.docker.com/products/docker-desktop/")
    info = docker("info", capture=True)
    if info.returncode != 0:
        if "permission denied" in info.stderr.lower():
            sys.exit("Nincs jogod a Docker használatához. Linuxon add hozzá magad a docker csoporthoz:\n"
                     "  sudo usermod -aG docker $USER\n"
                     "majd jelentkezz ki és be, és próbáld újra.")
        sys.exit("A Docker nem fut. Indítsd el a Docker Desktopot, várd meg, amíg elindul, és próbáld újra.")


def native_linux_engine():
    """Docker Engine directly on Linux (not Docker Desktop): containers can share the host network."""
    if not sys.platform.startswith("linux"):
        return False
    result = docker("info", "--format", "{{.OperatingSystem}}", capture=True)
    return "Docker Desktop" not in result.stdout


def running():
    result = docker("ps", "-q", "-f", f"name=^{CONTAINER}$", capture=True)
    return bool(result.stdout.strip())


def require_running():
    if not running():
        sys.exit("A tesztek nem futnak. Indítsd el őket: python tests.py start <NEPTUN>")


def valid_neptun(neptun):
    if len(neptun) != 6 or not neptun.isalnum():
        sys.exit("A Neptun-kód 6 betűből/számból áll, pl.: python tests.py start ABC123")


def launch(neptun, tasks):
    """Starts the container with the given tasks' test servers; returns once it runs."""
    if not os.path.isdir(TESTS_DIR):
        sys.exit(f"Nem találom a tesztek mappáját: {TESTS_DIR}")
    docker("rm", "-f", CONTAINER, capture=True)
    print("A tesztek indítása (az első alkalommal a Docker letölti a szükséges képet, ez eltarthat egy ideig)...")
    if native_linux_engine():
        # the container shares the host network, so it reaches servers bound to localhost too
        network = ["--network", "host", "-e", "TELKOM_HOST=127.0.0.1"]
    else:
        # Docker Desktop (Windows, macOS): publish the test ports, reach the host by name
        network = ["-p", f"127.0.0.1:{PORTS}:{PORTS}", "-p", f"127.0.0.1:{PORTS}:{PORTS}/udp",
                   "--add-host", "host.docker.internal:host-gateway"]
    # the obfuscated tests contain an x86-64 runtime: Apple Silicon Macs run the image emulated
    result = docker("run", "-d", "--name", CONTAINER, "--platform", "linux/amd64", *network,
                    "-v", f"{TESTS_DIR}:/tesztek:ro",
                    "-e", f"NEPTUN={neptun.upper()}",
                    "-w", "/tesztek",
                    IMAGE, "python", "-u", "run.py", "servers", *tasks, capture=True)
    if result.returncode != 0:
        sys.exit("Nem sikerült elindítani a teszteket:\n" + result.stderr)
    time.sleep(2)
    if not running():
        output = docker("logs", CONTAINER, capture=True)
        docker("rm", "-f", CONTAINER, capture=True)
        text = output.stdout + output.stderr
        if "can't open file '/tesztek/run.py'" in text:
            sys.exit("A Docker nem látja a tesztek mappáját. Ha a Docker snapből van telepítve (Ubuntu),\n"
                     "akkor csak a home mappádban lévő fájlokat éri el: másold a csomagot a home mappádba,\n"
                     "és onnan indítsd.")
        if "pyarmor_runtime.so: cannot open shared object file" in text:
            # the runtime is x86-64: the container ran on another CPU type (e.g. ARM) without emulation
            sys.exit("A tesztek csak x86-64 (Intel) módban futnak, de a Docker más processzor-módban indította el\n"
                     "őket. Apple Silicon (M1/M2/M3...) Macen a Docker Desktopban kapcsold be:\n"
                     "Settings -> General -> Use Rosetta for x86_64/amd64 emulation on Apple Silicon,\n"
                     "indítsd újra a Docker Desktopot, és próbáld újra.")
        sys.exit("A tesztek nem tudtak elindulni:\n" + output.stdout + output.stderr)


def start(neptun, tasks):
    valid_neptun(neptun)
    launch(neptun, tasks)
    which = ", ".join(tasks) if tasks else "minden feladat"
    print(f"A tesztek futnak ({neptun.upper()}; {which}).")
    print("Ebben az ablakban látod a tesztszerverek naplóját. A saját programjaidat és a")
    print("'python tests.py test ...' parancsot egy MÁSIK ablakban futtasd. Leállítás: Ctrl+C\n", flush=True)
    try:
        stream_docker("logs", "-f", CONTAINER)
    except KeyboardInterrupt:
        pass
    finally:
        stop()


def show_log():
    require_running()
    stream_docker("logs", "-f", CONTAINER)


def test(task, port):
    require_running()
    stream_docker("exec", CONTAINER, "python", "-u", "run.py", "test", task, port)


def stop(quiet=False):
    docker("rm", "-f", CONTAINER, capture=True)
    if not quiet:
        print("A tesztek leálltak.")


# ---------------------------------------------------------------- check: everything in one command

def log_lines():
    return docker("logs", CONTAINER, capture=True).stdout.splitlines()


def wait_ready(task, spec):
    """The control socket is bound before the first log line; a test server logs once it listens."""
    for _ in range(300):
        lines = log_lines()
        needed = spec.get("ready", [f"{task} tesztszerver"] if "port" in spec else [])
        if lines and all(any(f"[{tag}" in line for line in lines) for tag in needed):
            return
        time.sleep(0.2)
    sys.exit("A tesztek nem indultak el időben. Próbáld újra.")


def free_port():
    with socket.socket() as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def port_taken(port):
    """True once some program has bound the port. A bind attempt, not a connection: a test
    connection could crash a simple server that expects a whole message from every client."""
    for addr in ("0.0.0.0", "127.0.0.1"):
        s = socket.socket()
        try:
            s.bind((addr, port))
        except OSError:
            return True
        finally:
            s.close()
    return False


def tail(path, lines=12):
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read().strip().splitlines()
    except OSError:
        return ""
    return "\n".join("      " + line for line in text[-lines:])


def program(path, explicit, kind):
    full = os.path.abspath(path if explicit else os.path.join(HERE, path))
    if not os.path.isfile(full):
        hint = "" if explicit else f", vagy add meg így: --{kind} <fájl>"
        sys.exit(f"Nem találom a programodat: {full}\nTedd ide{hint}")
    return full


def check_server(task, spec, path, codes, problems):
    port = free_port()
    args = [a.format(port=port) for a in spec.get("server_args", ["{port}"])]
    out_path = os.path.join(HERE, f".{task}_szerver_kimenet.txt")
    print(f"A szervered indítása: python {os.path.basename(path)} {' '.join(args)}")
    with open(out_path, "w", encoding="utf-8") as out:
        proc = subprocess.Popen([sys.executable, "-u", path, *args], cwd=os.path.dirname(path),
                                stdin=subprocess.DEVNULL, stdout=out, stderr=subprocess.STDOUT, env=CHILD_ENV)
    try:
        deadline = time.time() + SERVER_START
        while not port_taken(port):
            if proc.poll() is not None:
                problems.append("a szervered elindulás közben kilépett")
                print("  HIBA – a szervered elindulás közben kilépett. A kimenete:\n" + tail(out_path))
                return
            if time.time() > deadline:
                problems.append("a szervered nem kezdett figyelni")
                print(f"  HIBA – a szervered {SERVER_START} másodperc alatt nem kezdett figyelni a {port} porton.\n"
                      "  A portot parancssori paraméterként kapja: sys.argv-ből olvasd ki!")
                return
            time.sleep(0.1)
        time.sleep(0.3)                                    # bind() done; give listen() a moment
        result = docker("exec", CONTAINER, "python", "-u", "run.py", "test", task, str(port), capture=True)
        for line in (result.stdout + result.stderr).splitlines():
            print("  " + line)
            match = re.search(r"RENDBEN, kód: (\w+)", line)
            if match:
                codes.append(match.group(1))
            elif "HIBA" in line:
                problems.append(line.split(":")[0])
        if proc.poll() is not None:
            problems.append("a szervered leállt a tesztek közben")
            print("  A szervered leállt a tesztek közben. A kimenete:\n" + tail(out_path))
    finally:
        if proc.poll() is None:
            proc.terminate()
            try:
                proc.wait(timeout=3)
            except subprocess.TimeoutExpired:
                proc.kill()
        try:
            os.remove(out_path)
        except OSError:
            pass


def check_client(task, spec, path, neptun, codes, problems):
    before = len(log_lines())
    outputs = []
    for run in spec["runs"]:
        args = ["localhost", str(spec["port"]), *[a.format(neptun=neptun) for a in run]]
        print(f"A kliensed: python {os.path.basename(path)} {' '.join(args)}")
        try:
            result = subprocess.run([sys.executable, "-u", path, *args], cwd=os.path.dirname(path),
                                    stdin=subprocess.DEVNULL, capture_output=True, text=True, encoding="utf-8", errors="replace",
                                    timeout=spec.get("timeout", CLIENT_RUN), env=CHILD_ENV)
        except subprocess.TimeoutExpired:
            problems.append("a kliensed nem lépett ki")
            print(f"  HIBA – a kliensed {spec.get('timeout', CLIENT_RUN)} másodperc alatt nem lépett ki")
            continue
        outputs.append(result.stdout)
        shown = (result.stdout.strip().splitlines() or ["(nem írt ki semmit)"])[-1]
        print("  kiírta: " + shown)
        if result.returncode != 0:
            if "EOFError" in result.stderr:
                print("  a kliensed input()-ra várt: minden adatot parancssori paraméterként kap (sys.argv)")
            print("  a kliensed hibával lépett ki:\n" + "\n".join(
                "      " + line for line in result.stderr.strip().splitlines()[-8:]))
    time.sleep(0.5)
    new = [line for line in log_lines()[before:] if line.startswith(f"[{task} tesztszerver")]
    print("A tesztszerver naplója:")
    for line in new or ["(a kliensed nem kapcsolódott a tesztszerverhez)"]:
        print("  " + line.split("] ", 1)[-1])
    success = [line for line in new if "RENDBEN" in line]
    if not success:
        problems.append("a kliens tesztje")
        return
    match = re.search(r"(?:Kód a kvízhez|fél pontot ér): (\w+)|-> ([\d.]+) –|a válasz: ([\d.]+:\d+) –", success[-1])
    code = match and next(group for group in match.groups() if group)
    if task in ("dns", "stun") and code and not any(code in out for out in outputs):
        problems.append("a kliens nem írta ki a kapott IP-címet")
        print(f"  A tesztszerver a {code} címet küldte, de a kliensed nem írta ki.")
        return
    if code:
        codes.append(code)


def check(neptun, task, server_file, client_file):
    valid_neptun(neptun)
    neptun = neptun.upper()
    spec = CHECKS.get(task)
    if spec is None:
        sys.exit("Ismeretlen feladat. Ezek vannak: " + ", ".join(CHECKS))
    server = program(server_file or spec["server"], bool(server_file), "server") if "server" in spec else None
    client = program(client_file or spec["client"], bool(client_file), "client") if "client" in spec else None
    if running():
        sys.exit("A tesztek már futnak (pl. egy 'start' ablakban). Állítsd le őket (Ctrl+C abban az\n"
                 "ablakban, vagy: python tests.py stop): a check maga indítja és állítja le őket.")
    codes, problems = [], []
    try:
        launch(neptun, [task])                              # inside try: a Ctrl+C here still cleans up
        wait_ready(task, spec)
        if server:
            print("\n== A szervered tesztje")
            check_server(task, spec, server, codes, problems)
        if client:
            print("\n== A kliensed tesztje")
            check_client(task, spec, client, neptun, codes, problems)
    except KeyboardInterrupt:
        problems.append("megszakítva")
        print("\nMegszakítva.")
    finally:
        signal.signal(signal.SIGINT, signal.SIG_IGN)       # a second Ctrl+C must not stop the cleanup
        stop(quiet=True)
    print("\n== Összesítés")
    if codes:
        print("A kódok a Moodle kvízhez: " + "  ".join(codes))
    print("Minden teszt rendben." if not problems else f"Nem sikerült: {len(problems)} teszt / lépés.")


def main():
    args = sys.argv[1:]
    if args[:1] == ["check"] and len(args) >= 3:
        options = {"--server": None, "--client": None}
        rest = args[3:]
        while rest:
            if rest[0] not in options or len(rest) < 2:
                print(__doc__)
                return
            options[rest[0]] = rest[1]
            rest = rest[2:]
        check_docker()
        check(args[1], args[2], options["--server"], options["--client"])
    elif args[:1] == ["start"] and len(args) >= 2:
        check_docker()
        start(args[1], args[2:])
    elif args == ["log"]:
        check_docker()
        show_log()
    elif args[:1] == ["test"] and len(args) == 3 and args[2].isdigit():
        check_docker()
        test(args[1], args[2])
    elif args == ["stop"]:
        check_docker()
        stop()
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
