#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>

void handler(int signumber){
    printf("Handler vagyok, ez a signal jött: %i\n",signumber);
}

struct uzenet{
    long mtype;
    char mtext[1024];
};

int main(int argc, char* argv[])
{
    signal(SIGTERM,handler);

    int pipe_tiszt_rendor[2];
    pipe(pipe_tiszt_rendor);

    key_t kulcs = ftok(argv[0],1);
    int uzenetsor = msgget(kulcs, 0600 | IPC_CREAT);

    int oszt_mem_id = shmget(kulcs,sizeof(int),IPC_CREAT|S_IRUSR|S_IWUSR);
    int* s = shmat(oszt_mem_id,NULL,0);

    char* sem_nev="/RendkívülienOltárianEgyediSzemaforNév";
    sem_t* sem_id=sem_open(sem_nev,O_CREAT,S_IRUSR|S_IWUSR,0);

    pid_t rendor = fork();
    if (rendor == 0)
    {
        shmdt(s);
        printf("Várakozás hogy a rendőr elfoglalja a helyét\n");
        sleep(1);
        printf("Rendőr: Jelentem hogy elfoglaltam a helyem\n");
        kill(getppid(),SIGTERM);

        char buffer[100];
        read(pipe_tiszt_rendor[0],buffer,sizeof(buffer));
        printf("Rendőr ezt olvasta a csobol: %s\n",buffer);
        close(pipe_tiszt_rendor[0]);

        char* valasz = "Igen kötelező, ha elhagyja a lakást!";
        printf("Rendőr írja a csőbe: %s\n",valasz);
        write(pipe_tiszt_rendor[1],valasz,strlen(valasz) + 1);
        close(pipe_tiszt_rendor[1]);

        exit(0);
    }

    pid_t foorvos = fork();  
    if(foorvos == 0)
    {
        close(pipe_tiszt_rendor[0]);
        close(pipe_tiszt_rendor[1]);
        printf("Várakozás hogy a főorvos elfoglalja a helyét\n");
        sleep(2);
        printf("Főorvos: Jelentem hogy elfoglaltam a helyem\n");
        kill(getppid(),SIGTERM);

        const struct uzenet uz = { 5, "A maszk viselése valóban nagyon fontos, védve a többi embert és magunkat is a boltokban és az utakon." };
        sleep(2);
        printf("Főorvos uzenetsorba irt\n");
        msgsnd(uzenetsor,&uz,strlen(uz.mtext) + 1,0);

        sleep(1);
        int fertozottekSzama = 20;
        printf("Főorvos beleirta az osztott memóriába a fertozottek szamat\n");
        *s = fertozottekSzama;
        sem_post(sem_id);

        shmdt(s);

        exit(0);
    }
    
    //Kommunikációs Tiszt

    pause();
    printf("Jel megjött, rendőr elfoglalta a helyét\n");

    pause();
    printf("Jel megjött, főorvos elfoglalta a helyét\n");

    char* kerdes = "Maszk viselése kötelező minden boltban?";
    printf("Tiszt írja a csőbe: %s\n",kerdes);
    write(pipe_tiszt_rendor[1],kerdes,strlen(kerdes) + 1);
    close(pipe_tiszt_rendor[1]);

    sleep(1);

    char buffer[100];
    read(pipe_tiszt_rendor[0],buffer,sizeof(buffer));
    printf("Tiszt ezt olvasta a csőből: %s\n",buffer);
    close(pipe_tiszt_rendor[0]);

    struct uzenet uz;
    msgrcv(uzenetsor,&uz,1024,5,0);
    printf("Tiszt ezt olvassa az üzenetsorrol: %s\n",uz.mtext);

    msgctl(uzenetsor,IPC_RMID,NULL);

    sem_wait(sem_id);
    printf("Tiszt kiolvassa az osztott memóriából a fertőzöttek számát: %i\n",*s);
    sem_post(sem_id);

    shmdt(s);

    waitpid(rendor,NULL,0);
    waitpid(foorvos,NULL,0);
    
    return 0;
}