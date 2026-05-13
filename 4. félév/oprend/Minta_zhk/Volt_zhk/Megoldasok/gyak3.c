#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

char* whoami;
struct msg{
 long type;
 int ora;
 int datum;
};


void dummy(int x){}

int main(int argc, char* argv[]){
    key_t key = ftok(argv[0],370);
    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0);

    pid_t gy = fork();
    if(gy){
        whoami = "szulo";

        struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
        semop(sem, &muvelet, 1);
        
        struct msg* shared = shmat(shm,NULL,0);
        printf("[%s] Kaptam az apptól: óraállás: %d, dátum (év): %d\n", whoami, shared->ora, shared->datum);
        shmdt(shared);

        waitpid(gy, 0, 0);
        shmctl(shm, IPC_RMID, NULL);
        semctl(sem, 0, IPC_RMID);
    }else{
        int fd[2];
        pipe(fd);
        pid_t unoka = fork();
        if(unoka){
            whoami = "gyerek";
            close(fd[0]);

            sleep(1);
            kill(unoka, SIGUSR1);
            printf("[%s]: jelzest küldtem!\n", whoami);
            
            write(fd[1], "azon: qj6007, jelszo: alma", 27);
            close(fd[1]);

            sleep(1);
            char* shared = shmat(shm,NULL,0);
            printf("[%s] Kaptam az apptól: %s\n", whoami, shared);
            shmdt(shared);

            if(rand() % 2 || 1 == 1){
                printf("[%s] jó\n", whoami);
            }else{
                printf("[%s] nem jó\n", whoami);
            }

            //MINDIG kell küldeni!
            struct msg to_send = {.type = 5, .ora = rand(), .datum = 2024};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            waitpid(unoka, 0, 0);
            msgctl(msgQ, IPC_RMID, NULL);
        }else{
            whoami = "unoka";
            signal(SIGUSR1, dummy);
            close(fd[1]);

            pause();
            printf("[%s]: jelzest kaptam!\n", whoami);

            char buf[27];
            read(fd[0], buf, 27);
            close(fd[0]);
            printf("[%s] adatok: %s\n", whoami, buf);

            char* shared = shmat(shm,NULL,0);
            strcpy(shared, "Ez az óraállás képe!");

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            printf("[%s] óraállás: %d, dátum (év): %d\n", whoami, to_receive.ora, to_receive.datum);

            memcpy(shared, &to_receive, sizeof(to_receive));
            shmdt(shared);
            struct sembuf muvelet = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
        }
    }
    return 0;
}