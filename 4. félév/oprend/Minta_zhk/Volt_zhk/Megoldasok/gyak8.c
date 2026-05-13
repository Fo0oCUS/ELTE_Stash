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
#define MAX_PER_PLANE 239
#define two_plane_condition (utasok > MAX_PER_PLANE)

struct msg{
 long type;
 char buf[64];
};

void dummy(int x){}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int utasok = atoi(argv[1]);
    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 1);

    int p_to_gy1[2];
    int p_to_gy2[2];
    pipe(p_to_gy1);
    pid_t gy1 = fork();

    if(gy1){
        pid_t gy2 = 0;
        pipe(p_to_gy2);

        if(two_plane_condition){
            gy2 = fork();
            if(!gy2){
                whoami = "gép 2";
                close(p_to_gy1[0]);
                close(p_to_gy1[1]);
                close(p_to_gy2[1]);
                sleep(2);
                kill(getppid(), SIGUSR1);

                int my_utasok;
                read(p_to_gy2[0], &my_utasok, sizeof(my_utasok));
                pt("%d utas van a fedélzeten, várható utazási idő: 8 óra", my_utasok);

                struct msg to_send = {.type = 5, .buf = "A második gép megérkezett!"};
                msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

                int my_utasok2;
                read(p_to_gy2[0], &my_utasok2, sizeof(my_utasok2));
                close(p_to_gy2[0]);
                pt("A visszaúton ennyi utast viszek vissza: %d", my_utasok2);

                struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
                semop(sem, &muvelet, 1);
                char* shared = shmat(shm,NULL,0);
                sprintf(shared, "Oda: %d, vissza: %d", my_utasok, my_utasok2);
                shmdt(shared);
                muvelet.sem_op = 1;
                semop(sem, &muvelet, 1);
                return 0;
            }
        }

        whoami = "Foci Air / szülő";
        close(p_to_gy1[0]);
        if(two_plane_condition){
            close(p_to_gy2[0]);
        }
        signal(SIGUSR1, dummy);
        pt("%d utas van, %d gép indult", utasok, two_plane_condition ? 2 : 1);
        int utasok_1 = two_plane_condition ? MAX_PER_PLANE : utasok;
        int utasok_2 = (utasok - MAX_PER_PLANE) < 0 ? 0 : (utasok - MAX_PER_PLANE);
        if(utasok_2 > MAX_PER_PLANE){
            utasok_2 = MAX_PER_PLANE;
        }
        pt("Az első gépen %d utas utazik", utasok_1);
        if(two_plane_condition){
            pt("A másodikon pedig %d", utasok_2);
        }
        pause();
        if(two_plane_condition){
            pause();
        }
        
        write(p_to_gy1[1], &utasok_1, sizeof(utasok_1));

        if(two_plane_condition){
            write(p_to_gy2[1], &utasok_2, sizeof(utasok_2));
        }

        struct msg to_receive;
        msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
        pt("üzenetet kaptam: %s", to_receive.buf);
        if(two_plane_condition){
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("üzenetet kaptam: %s", to_receive.buf);
        }

        int visszautasok = rand() % 401 + 100;
        if(visszautasok < 239 && two_plane_condition){
            visszautasok = 240;
        }else if(!two_plane_condition && visszautasok > 239){
            visszautasok = 239;
        }
        
        utasok_1 = two_plane_condition ? MAX_PER_PLANE : visszautasok;
        write(p_to_gy1[1], &utasok_1, sizeof(utasok_1));
        close(p_to_gy1[1]);
        if(two_plane_condition){
            utasok_2 = visszautasok - MAX_PER_PLANE;
            write(p_to_gy2[1], &utasok_2, sizeof(utasok_2));
            close(p_to_gy2[1]);
        }

        waitpid(gy1, 0, 0);
        if(two_plane_condition){
            waitpid(gy2, 0, 0);
        }
        msgctl(msgQ, IPC_RMID, NULL);
        shmctl(shm, IPC_RMID, NULL);
        semctl(sem, 0, IPC_RMID);
    }else{
        whoami = "gép 1";
        close(p_to_gy1[1]);
        sleep(1);
        kill(getppid(), SIGUSR1);

        int my_utasok;
        read(p_to_gy1[0], &my_utasok, sizeof(my_utasok));
        pt("%d utas van a fedélzeten, várható utazási idő: 8 óra", my_utasok);

        struct msg to_send = {.type = 5, .buf = "Az első gép megérkezett!"};
        msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

        int my_utasok2;
        read(p_to_gy1[0], &my_utasok2, sizeof(my_utasok2));
        close(p_to_gy1[0]);
        pt("A visszaúton ennyi utast viszek vissza: %d", my_utasok2);

        struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
        semop(sem, &muvelet, 1);
        char* shared = shmat(shm,NULL,0);
        sprintf(shared, "Oda: %d, vissza: %d", my_utasok, my_utasok2);
        shmdt(shared);
        muvelet.sem_op = 1;
        semop(sem, &muvelet, 1);
    }
    return 0;
}