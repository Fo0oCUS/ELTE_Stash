#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

struct uzenet { 
    long mtype; //ez egy szabadon hasznalhato ertek, pl uzenetek osztalyozasara
    char mtext [ 1024 ]; 
}; 

int main(int argc, char* argv[]) { 

    key_t kulcs = ftok(argv[0],1); // kell egy egyedi kulcs, első paraméter egy file, a második egy kis szám vagy karakter (verzioszam)
    int uzenetsor = msgget( kulcs, 0600 | IPC_CREAT ); // létrehozzuk az üzenetsort

    pid_t child = fork(); // gyerek process létrehozása
    if(child == 0) // gyerek
    {
        struct uzenet uz;  // létrehozzuk a structot amibe fogunk olvasni
        msgrcv(uzenetsor, &uz, 1024, 5, 0); // olvassuk az üzenetet, az üzenetsorból, hova, mennyit, az uzenet osztalya, utána 0
        exit(0);
    }
    // szülő
    const struct uzenet uz = { 5, "Hajra Fradi!" }; // egy struct amit küldünk
    msgsnd(uzenetsor, &uz, strlen ( uz.mtext ) + 1 , 0); // elküldjük az üzenetet, üzenetsorba, honnan (pointer) mennyit, utána 0
    wait(NULL); // bevárjuk a gyerek process befejezését
    msgctl( uzenetsor, IPC_RMID, NULL ); // törli az üzenetsort
    return 0; 
} 
