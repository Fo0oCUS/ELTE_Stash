#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

void handler(int signumber){

}

int main(){
    signal(SIGTERM,handler); // handlerre való feliratkozás
    
    pid_t child = fork(); // gyerek process létrehozása
    if (child == 0) // gyerek
    { 
        sleep(1); // biztosítjuk hogy a szülő eljusson a pause-ig 
        kill(getppid(),SIGTERM); //SIGTERM signal küldése a szülőnek
        exit(0); // process vége
    }
    // szülő

    pause(); // vár egy signalig        
    wait(NULL); // megvárja míg befejeződik a gyerek process

    return 0;
}
