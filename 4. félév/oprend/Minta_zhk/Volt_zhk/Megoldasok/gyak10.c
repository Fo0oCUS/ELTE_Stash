#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <mqueue.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define pt(...) {printf("[%s] ", whoami ?: "N/A"); printf(__VA_ARGS__); printf("\n"); }
char* whoami = 0;

void dummy(int x){}

struct hely_es_ido{
    char hely[60];
    int ido;
};

struct msg{
 long type;
 char buf[64];
};

void szemafor_muvelet(int semid, int sem_op){
    struct sembuf muvelet = {.sem_num = 0, .sem_op = sem_op, .sem_flg = 0};
    semop(semid, &muvelet, 1);
}

int main(int argc, char* argv[]){
    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0);
    int shm = shmget(key, 200, S_IRUSR | S_IWUSR | IPC_CREAT);

    int sz_to_gy1[2];
    int sz_to_gy2[2];
    pipe(sz_to_gy1);

    pid_t gy1 = fork();
    if(gy1){
        pipe(sz_to_gy2);
        pid_t gy2 = fork();
        if(gy2){
            whoami = "oktató / szülő";
            close(sz_to_gy1[0]);
            close(sz_to_gy2[0]);
            signal(SIGUSR1, dummy);

            pause();
            pause();

            struct hely_es_ido hei = {.ido = atoi(argv[2])};
            strcpy(hei.hely, argv[1]);

            write(sz_to_gy1[1], &hei, sizeof(hei));
            close(sz_to_gy1[1]);
            write(sz_to_gy2[1], &hei, sizeof(hei));
            close(sz_to_gy2[1]);

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("Üzenet jött gy2-től: %s", to_receive.buf);

            char* shared = shmat(shm, NULL, 0);
            strcpy(shared, "Napi lista: ");
            strcat(shared, "Oktató: Picadilly Circus, Buckingham Palace, Hyde Park");
            szemafor_muvelet(sem, 1);
            
            sleep(1);
            szemafor_muvelet(sem, -3);
            pt("Ez van az osztott memóriában: %s", shared);

            shmdt(shared);

            waitpid(gy1, 0, 0);
            waitpid(gy2, 0, 0);
            msgctl(msgQ, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
            shmctl(shm, IPC_RMID, NULL);
        }else{
            whoami = "hallgató2 / gy2";
            close(sz_to_gy1[0]);
            close(sz_to_gy1[1]);
            close(sz_to_gy2[1]);
            sleep(1);
            kill(getppid(), SIGUSR1);

            struct hely_es_ido hei;
            read(sz_to_gy2[0], &hei, sizeof(hei));
            pt("Itt találkozunk: %s, ekkor: %d", hei.hely, hei.ido);

            pt("Hamarabb értem ide, túl nagy a tömeg");
            struct msg to_send = {.type = 5, .buf = "Nagy a tömeg, egy óra múlva a Trafalgár téren!"};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
            to_send.type = 6;
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            char* shared = shmat(shm, NULL, 0);
            szemafor_muvelet(sem, -2);
            strcat(shared, "\nGyerek 2: Westminster, London Eye, Soho");
            szemafor_muvelet(sem, 3);
            shmdt(shared);

            close(sz_to_gy2[0]);
        }
    }else{
        whoami = "hallgató1 / gy1";
        close(sz_to_gy1[1]);
        sleep(2);
        kill(getppid(), SIGUSR1);

        struct hely_es_ido hei;
        read(sz_to_gy1[0], &hei, sizeof(hei));
        pt("Itt találkozunk: %s, ekkor: %d", hei.hely, hei.ido);

        struct msg to_receive;
        msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 6, 0);
        pt("Üzenet jött gy2-től: %s", to_receive.buf);

        char* shared = shmat(shm, NULL, 0);
        szemafor_muvelet(sem, -1);
        strcat(shared, "\nGyerek 1: British Museum, Tower, Big Ben");
        szemafor_muvelet(sem, 2);
        shmdt(shared);

        close(sz_to_gy1[0]);
    }
    return 0;
}