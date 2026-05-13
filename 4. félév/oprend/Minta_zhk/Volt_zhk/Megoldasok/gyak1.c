#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int receivedsignal){}

struct msg {
 long type;
 char buf[180];
};

int main(int argc, char* argv[]){
    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0) ;

    int ra_kt[2];
    pipe(ra_kt);
    int kt_ra[2];
    pipe(kt_ra);
    pid_t ra = fork();

    if(ra){
        pid_t ot = fork();
        if(ot){
            //szulo
            close(ra_kt[0]);
            close(kt_ra[1]);
            signal(SIGUSR1, handler);
            pause();
            pause();

            char* m1 = "Maszk viselése kötelező minden boltban?";
            write(ra_kt[1], m1, strlen(m1) + 1);
            close(ra_kt[1]);
            
            
            char buf[80];
            read(kt_ra[0], buf, 80);
            printf("KT kapta: %s\n", buf);
            close(kt_ra[0]);

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            printf("KT kapta (OT-tól): %s\n", to_receive.buf);
            msgctl(msgQ, IPC_RMID, NULL);


            struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
            semop(sem, &muvelet, 1);

            char* shared = shmat(shm,NULL,0);
            int napi_uj_fertozottek_szama;
            memcpy(&napi_uj_fertozottek_szama, shared, sizeof(int));
            shmdt(shared);
            printf("napi_uj_fertozottek_szama: %d\n", napi_uj_fertozottek_szama);
            shmctl(shm, IPC_RMID, NULL);

            semctl(sem, 0, IPC_RMID);


            waitpid(ot, 0, 0);
            waitpid(ra, 0, 0);
            printf("KT kilép\n");
        }else{
            close(ra_kt[0]);
            close(ra_kt[1]);
            close(kt_ra[0]);
            close(kt_ra[1]);
            //ot
            sleep(1);
            kill(getppid(), SIGUSR1);
            
            struct msg to_send = {.type = 5, .buf = "A maszk viselése valóban nagyon fontos, védve a többi embert és magunkat is a boltokban és az utakon."};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);


            char* shared = shmat(shm,NULL,0);
            int napi_uj_fertozottek_szama = 900;
            memcpy(shared, &napi_uj_fertozottek_szama, sizeof(int));
            shmdt(shared);

            struct sembuf muvelet = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);


            printf("OT kilép\n");
        }
    }else{
        close(ra_kt[1]);
        close(kt_ra[0]);
        //ra
        sleep(2);
        kill(getppid(), SIGUSR1);
        
        char buf[80];
        read(ra_kt[0], buf, 80);
        printf("RA kapta: %s\n", buf);
        close(ra_kt[0]);

        char* m1 = "Igen kötelező, ha elhagyja a lakást!";
        write(kt_ra[1], m1, strlen(m1) + 1);
        close(kt_ra[1]);

        printf("RA kilép\n");
    }
    return 0;
}