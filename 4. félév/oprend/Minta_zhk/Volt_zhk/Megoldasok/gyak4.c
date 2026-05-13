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
#define pt(fstr, ...)                                              \
    {                                                              \
        printf("[%s] " fstr "\n", whoami ?: "N/A", ##__VA_ARGS__); \
    }

char *whoami = 0;

void dummy_handler(int x) {}
void do_not_kill_me(int signum) { signal(signum, dummy_handler); }

void szemafor_muvelet(int semid, int sem_op)
{
    struct sembuf muvelet = {.sem_num = 0, .sem_op = sem_op, .sem_flg = 0};
    semop(semid, &muvelet, 1);
}

struct msg
{
    long type;
    char buf[64];
};

int main(int argc, char *argv[])
{
    key_t key = ftok(argv[0], 37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);

    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);

    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0);

    do_not_kill_me(SIGUSR1);

    int pincer_asztal[2]; // 0: read
    pipe(pincer_asztal);

    int fonok_pincer[2];
    pipe(fonok_pincer);

    int pincer_fonok[2];
    pipe(pincer_fonok);

    pid_t gy1 = fork();
    if (gy1)
    {
        pid_t gy2 = fork();
        if (gy2)
        {
            whoami = "Főnök";
            close(fonok_pincer[0]);
            close(pincer_fonok[1]);
            pause();

            char *msg = "vendég érkezett, el ne szalaszd nekem!";
            write(fonok_pincer[1], msg, strlen(msg) + 1);
            char buf[100];
            read(pincer_fonok[0], buf, sizeof(buf));
            pt("a pincérünk igyekszik: %s", buf);
            close(fonok_pincer[1]);

            read(pincer_fonok[0], buf, sizeof(buf));
            pt("kiírom a pincér csöves üzenetét: %s", buf);

            close(pincer_fonok[0]);

            wait(0);
            wait(0);
            msgctl(msgQ, IPC_RMID, NULL);
            shmctl(shm, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
        }
        else
        {
            whoami = "Asztal";
            close(pincer_asztal[1]);
            close(fonok_pincer[0]);
            close(fonok_pincer[1]);
            close(pincer_fonok[0]);
            close(pincer_fonok[1]);
            sleep(1);
            kill(getppid(), SIGUSR1);
            kill(gy1, SIGUSR1);
            
            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("kaptam a pincértől: %s", to_receive.buf);
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("kaptam a pincértől: %s", to_receive.buf);

            char *choice = ((char *[]){"Tavaszi tekercs", "Epres üzenet", "Nyári sült csövecskék"})[rand() % 3];
            int amount = rand() % 2 ? 2 : 3;

            struct msg to_send = (struct msg) {.type = 6};
            sprintf(to_send.buf, "Kérek %s-ból/ből ennyit: %i", choice, amount);
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            char* shared = shmat(shm,NULL,0);
            szemafor_muvelet(sem, -1);
            pt("meghozták a számlát: %s", shared);
            pt("hogy mennyi???? ide se jövök többet (%s)!", shared);
            shmdt(shared);
        }
        pt("kilepes");
    }
    else
    {
        whoami = "Pincér";
        close(fonok_pincer[1]);
        close(pincer_fonok[0]);
        pause();
        struct msg to_send = {.type = 5, .buf = "mit parancsol a kedves vendég?"};
        msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
        strcpy(to_send.buf, "Tavaszi tekercs, Epres üzenet, Nyári sült csövecskék");
        msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

        char buf[100];
        read(fonok_pincer[0], buf, sizeof(buf));
        pt("a főnök ideges: %s", buf);
        char* msg = "nyugi főnök, rajta vagyok az ügyön!";
        write(pincer_fonok[1], msg, strlen(msg) + 1);
        close(fonok_pincer[0]);

        struct msg to_receive;
        msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 6, 0);
        pt("rendelés jött az asztaltól: %s", to_receive.buf);

        msg = "ezt elkaptuk főnök, rendelt";
        sleep(1);
        write(pincer_fonok[1], msg, strlen(msg) + 1);
        close(pincer_fonok[1]);

        char* shared = shmat(shm,NULL,0);
        sprintf(shared, "ennyi az annyi: %i", rand() % 9001 + 1000);
        szemafor_muvelet(sem, 1);
        shmdt(shared);

        pt("kilepes");
    }
    return 0;
}