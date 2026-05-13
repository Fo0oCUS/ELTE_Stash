#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2]; // csőnek kell 2 elemű int tömb
    char sz[100];  // egy buffer az olvasáshoz
    pipe(pipefd); // létrehozzuk a csövet

    pid_t pid = fork();	// gyerek process létrehozása

    if (pid == 0) // gyerek
	{
        close(pipefd[1]); // bezárjuk a csőnek az író végét
        read(pipefd[0],sz,sizeof(sz)); // olvasás a csőből, ugye olvasó részéből olvasunk mibe mennyit
        close(pipefd[0]); // bezárjuk az olvasó részét is
        exit(0); // gyerek process befejezése
    } 
    // szülő

    close(pipefd[0]); // bezárjuk az olvasó részét
    write(pipefd[1], "Pointer kell ide",17); // beelírunk a csőbe, ugye az író részébe mit és mennyit
    close(pipefd[1]); // bezárjuk az író részét

    wait(NULL);// megvárjuk míg a gyerek process befejeződik

	return 0;
}

 
