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

struct msg{
 long type;
 int betegek, kapott_oltast;
};

int main(int argc, char* argv[]){
    key_t key = ftok(argv[0],37);
    int gy1nek[2];
    int gy2nek[2];
    pipe(gy1nek);
    pipe(gy2nek);

    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 1);

    pid_t gy1 = fork();
    if(gy1){
        pid_t gy2 = fork();
        if(gy2){
            whoami = "szülő / Dr. Bubó";
            close(gy1nek[0]);
            close(gy2nek[0]);
            signal(SIGUSR1, dummy);
            pause();
            pause();
            int paciensek = atoi(argv[1]);
            int ursula = paciensek / 2;
            int csormester = paciensek - ursula;

            write(gy1nek[1], &ursula, sizeof(ursula));
            close(gy1nek[1]);
            write(gy2nek[1], &csormester, sizeof(csormester));
            close(gy2nek[1]);

            int kaptak = 0, nem_kaptak = 0;
            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("ennyien voltak betegek: %d, ennyien kaptak tehát oltást: %d", to_receive.betegek, to_receive.kapott_oltast);
            nem_kaptak += to_receive.betegek;
            kaptak += to_receive.kapott_oltast;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            nem_kaptak += to_receive.betegek;
            kaptak += to_receive.kapott_oltast;
            pt("ennyien voltak betegek: %d, ennyien kaptak tehát oltást: %d", to_receive.betegek, to_receive.kapott_oltast);

            struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
            int logfile_fd = open("logfile.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            char buf[90];
            sprintf(buf, "Ma ennyien kaptak oltást: %d és ennyien nem: %d\n", kaptak, nem_kaptak);
            write(logfile_fd, buf, strlen(buf));
            close(logfile_fd);
            muvelet = (struct sembuf){.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);

            waitpid(gy1, 0, 0);
            waitpid(gy2, 0, 0);
            msgctl(msgQ, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
        }else{
            whoami = "Csőrmester";
            close(gy1nek[0]);
            close(gy1nek[1]);
            close(gy2nek[1]);
            sleep(1);
            kill(getppid(), SIGUSR1);

            int oltani;
            read(gy2nek[0], &oltani, sizeof(oltani));
            close(gy2nek[0]);
            pt("ennyit oltok: %d", oltani);

            struct msg to_send = {.type = 5, .betegek = oltani / 5, .kapott_oltast = oltani - (oltani / 5)};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
        }
    }else{
        whoami = "asszisztens / Ursula";
        close(gy2nek[0]);
        close(gy2nek[1]);
        close(gy1nek[1]);
        sleep(2);
        kill(getppid(), SIGUSR1);
        
        int oltani;
        read(gy1nek[0], &oltani, sizeof(oltani));
        close(gy1nek[0]);
        pt("ennyit oltok: %d", oltani);

        struct msg to_send = {.type = 5, .betegek = oltani / 5, .kapott_oltast = oltani - (oltani / 5)};
        msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
    }
    return 0;
}