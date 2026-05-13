#include <stdlib.h> // exit
#include <signal.h> // sigaction
#include <sys/wait.h> // wait
#include <unistd.h> // fork

void handler(int signumber){

}

int main(){

  struct sigaction sigact; // sigaction létrehozása
  sigact.sa_handler=handler; // handler beállítása
  sigemptyset(&sigact.sa_mask); // üresre inicializálja az adattagok, handler alatt nem blokkol plusz signalokat
  sigact.sa_flags=0; // default mód
  // sigaddset(&sigact.sa_mask,SIGUSR1) hozzá adja a SIGUSR1-t ami a handler lefutása alatt blokkolva lesz

  sigaction(SIGTERM,&sigact,NULL); // SIGTERM signal hozzáadása, azaz SIGTERM esetén a handler lefut
  sigaction(SIGUSR1,&sigact,NULL); // SIGUSR1 signal hozzáadása, azaz SIGUSR1 esetén a handler lefut

  pid_t child=fork();

  if (child == 0) // gyerek
  {
    sleep(1); // kill előtt mindig sleep, ezzel biztosítjuk hogy aki várja signalt az eljutott tényleg a várakozásig pause/sigsuspend
    kill(getppid(),SIGUSR1); // SIGUSR1 signalt küld a szülőnek

    sleep(1);
    kill(getppid(),SIGTERM); // SIGTERM signalt küld a szülőnek

    exit(0);
  }

  // szülő

  sigset_t sigset; // signal halmaz létrehozása
  sigfillset(&sigset); // minden signal legyen blokkolva
  sigdelset(&sigset,SIGTERM); // kiveszi a SIGTERM-et a blokkolt signalok közül
  sigsuspend(&sigset); // vár egy nem blokkolt signalra azaz (SIGTERM)-re mivel kivettük a blokkolt signalok közül

  wait(NULL);

  return 0;
}
