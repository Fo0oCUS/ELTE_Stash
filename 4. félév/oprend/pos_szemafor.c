#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MEMSIZE 1024

int main (int argc,char* argv[]) {
    char* sem_nev="/RendkívülienOltárianEgyediSzemaforNév";

    key_t kulcs=ftok(argv[0],1); // kell egy egyedi kulcs, első paraméter egy file, a második egy kis szám vagy karakter (verzioszam)
    int sh_mem_id=shmget(kulcs,MEMSIZE,IPC_CREAT|S_IRUSR|S_IWUSR); // osztott memoria letrehozasa, irasra olvasasra, 1024 mérettel
    char *s = shmat(sh_mem_id,NULL,0); // csatlakozás az osztott memoriához (elején ha int* akkor intet írunk bele, ha myStruct* akkor structot)
    // a 2. parameter akkor kell, ha sajat cimhez akarjuk illeszteni
    // a 3. paraméter lehet SHM_RDONLY, ekkor csak read van   
    
    sem_t *semid=sem_open(sem_nev,O_CREAT,S_IRUSR|S_IWUSR,0); // szemafor létrehozása utolso paraméter szemafor kezdőérték

    pid_t child = fork();
    if ( child == 0 ) //child
	{	
       	//  kritikus szakasz kezdete
		printf("Gyerek: Indul a posix szemafor down!\n");
       	sem_wait(semid); // ébred ha a szemafor helyek száma > 0 , kivon belőle 1-et és tovább engedi a processt, amíg 0 addig vár
       	printf("Gyerek, down rendben, eredmeny: %s",s);  
       	sem_post(semid); // növeli a szemafor értékét, felébreszthet egy várakozó processzt
       	//  kritikus szakasz vége/  	  
       	shmdt(s); // elengedjuk az osztott memoriat 
        exit(0);
    }
    
    char buffer[] = "Hajra Fradi!\n";
	printf("Szulo indul, kozos memoriaba irja: %s\n",buffer);
    sleep(4); // várunk hogy a szemafor érvényesüljön, mert a gyerek elér a waitig mivel 0 kezdőértékkel hoztuk létre a szemafort           
    strcpy(s,buffer);	// írás az osztott memóriába
	printf("Szulo, szemafor up!\n");
    sem_post(semid);	// növeli a szemafor értékét, felébreszthet egy várakozó processzt
	shmdt(s);	// elengedjuk az osztott memoriat 
    wait(NULL); // bevárjuk a gyereket
    sem_unlink(sem_nev); // törli név alapján a szemafort
	shmctl(sh_mem_id,IPC_RMID,NULL); // törölük az osztott memóriát
	
    return 0;
}


