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

void ignore(int s){}

struct msg {
 long type;
 char buf[64];
};

char* whoami;

void advanced_handler(int signumber, siginfo_t *info, void* unused){
    if(info->si_value.sival_int){
        //printf("Szuper! Még van %d db!\n", info->si_value.sival_int);
        printf("[%s] Köszönöm, kérek egyet!\n", whoami);
    }else{
        printf("[%s] Kár, hogy elfogyott!\n", whoami);
    }
}


int main(int argc, char* argv[]){
    srand(time(NULL));
    int sz_gy1[2];
    int sz_gy2[2];
    pipe(sz_gy1);
    pipe(sz_gy2);

    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);

    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0);

    char* aruk[] = {
        "kifli",
        "tej",
        "sajt"
    };

    struct sigaction sigact_usr1 = {
        .sa_sigaction = advanced_handler, //nem sa_handler
        .sa_flags = SA_SIGINFO //ez már többet tud
    };
    sigemptyset(&sigact_usr1.sa_mask);
    sigaction(SIGUSR2, &sigact_usr1,NULL);

    pid_t gy1 = fork();
    if(gy1){
        pid_t gy2 = fork();
        if(gy2){
            //szulo
            whoami = "szulo";
            close(sz_gy1[0]);
            close(sz_gy2[0]);
            signal(SIGUSR1, ignore);
            pause();
            pause();

            write(sz_gy1[1], "Kinyitott a bolt, bejöhetsz\n", 30);
            close(sz_gy1[1]);
            write(sz_gy2[1], "Kinyitott a bolt, bejöhetsz\n", 30);
            close(sz_gy2[1]);

            struct msg to_send = {.type = 5, .buf = "Szia!"};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
            to_send.type = 6;
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);


            char* shared = shmat(shm,NULL,0);
            struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
            printf("gy2 kér: %s\n", shared);
            muvelet.sem_op = 2;
            semop(sem, &muvelet, 1);

            muvelet.sem_op = -3;
            semop(sem, &muvelet, 1);
            printf("gy1 kér: %s\n", shared);

            sleep(1);
            union sigval s_value_ptr = {.sival_int = rand() % 3};
            printf("[%s] gy1-nek küldtem ennyi db van: %d\n", whoami, s_value_ptr.sival_int);
            sigqueue(gy1,SIGUSR2,s_value_ptr);
            s_value_ptr.sival_int = rand() % 3;
            printf("[%s] gy2-nek küldtem ennyi db van: %d\n", whoami, s_value_ptr.sival_int);
            sigqueue(gy2,SIGUSR2,s_value_ptr);


            waitpid(gy1, 0, 0);
            waitpid(gy2, 0, 0);
            shmdt(shared);
            msgctl(msgQ, IPC_RMID, NULL);
            shmctl(shm, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
            printf("[%s] kilep\n", whoami);
        }else{
            //gy2
            whoami = "gy2";
            close(sz_gy1[0]);
            close(sz_gy1[1]);
            close(sz_gy2[1]);
            sleep(1);
            kill(getppid(), SIGUSR1);

            char buf[30];
            read(sz_gy2[0], buf, 30);
            close(sz_gy2[0]);
            printf("[%s] Szulotol kaptam: %s\n", whoami, buf);

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 6, 0);
            printf("[%s] Szulotol kaptam koszones: %s\n", whoami, to_receive.buf);

            char* shared = shmat(shm,NULL,0);
            strcpy(shared, aruk[rand() % 3]);
            shmdt(shared);

            struct sembuf muvelet = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);

            pause();

            printf("[%s] kilep\n", whoami);
        }
    }else{
        //gy1
        whoami = "gy1";
        close(sz_gy2[0]);
        close(sz_gy2[1]);
        close(sz_gy1[1]);
        sleep(2);
        kill(getppid(), SIGUSR1);

        char buf[30];
        read(sz_gy1[0], buf, 30);
        close(sz_gy1[0]);
        printf("[%s] Szulotol kaptam: %s\n", whoami, buf);

        struct msg to_receive;
        msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
        printf("[%s] Szulotol kaptam koszones: %s\n", whoami, to_receive.buf);

        struct sembuf muvelet = {.sem_num = 0, .sem_op = -2, .sem_flg = 0};
        semop(sem, &muvelet, 1);
        char* shared = shmat(shm,NULL,0);
        strcpy(shared, aruk[rand() % 3]);
        shmdt(shared);
        muvelet.sem_op = 3;
        semop(sem, &muvelet, 1);

        pause();

        printf("[%s] kilep\n", whoami);
    }
    return 0;
}