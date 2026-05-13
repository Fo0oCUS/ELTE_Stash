#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

typedef struct {
    char nev[50];
    int kor;
} Ember;

int main (int argc,char* argv[]) {

    key_t kulcs=ftok(argv[0],1);  // kell egy egyedi kulcs, első paraméter egy file, a második egy kis szám vagy karakter (verzioszam)
    int oszt_mem_id=shmget(kulcs,sizeof(Ember),IPC_CREAT|S_IRUSR|S_IWUSR); // osztott memoria letrehozasa, irasra olvasasra, Ember struct mérettel
    Ember* s = shmat(oszt_mem_id,NULL,0); // csatlakozás az osztott memoriához (elején ha int* akkor intet írunk bele, ha myStruct* akkor structot)
    // a 2. parameter akkor kell, ha sajat cimhez akarjuk illeszteni
    // a 3. paraméter lehet SHM_RDONLY, ekkor csak read van   

    pid_t pid = fork(); // gyerek process létrehozása
    if(pid == 0) // gyerek
    {
        sleep(1);

        printf("A gyerek ezt olvasta az osztott memoriabol:\n");
        printf("Nev: %s\n", s->nev);
        printf("Kor: %d\n", s->kor);

        shmdt(s); // elengedjuk az osztott memoriat 
        exit(0);
    }
    // szülő
    strcpy(s->nev, "Bence"); // beirunk a memoriaba stringet csak stycpy-vel!!!
    s->kor = 21; // beirunk a memoriaba 
    
    shmdt(s); // elengedjuk az osztott memoriat 
    wait(NULL); 

    shmctl(oszt_mem_id,IPC_RMID,NULL);
    // IPC_RMID- torolni akarjuk a memoriat, ekkor nem kell 3. parameter
	// IPC_STAT- osztott memoria adatlekerdezes a 3. parameterbe,
	// ami shmid_ds struct tipusu mutato

   return 0;
}
