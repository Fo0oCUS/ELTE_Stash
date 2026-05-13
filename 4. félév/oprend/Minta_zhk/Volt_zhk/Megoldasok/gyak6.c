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

#define FO_TO_TAPSI 3
#define FO_TO_FULES 4
#define FULES_TO_FO 5
#define TAPSI_TO_FO 6

struct msg{
 long type;
 char buf[64];
};

struct szam_msg{
 long type;
 int nepszam;
};

char* Keress_meg = "Keress_meg";
char* Hazaterhet = "Hazaterhet";

int hazaterhetett_kapott;

void advanced_handler(int signumber, siginfo_t *info, void*unused){
    hazaterhetett_kapott = 0;
    if(Keress_meg == info->si_value.sival_ptr){
        //hazaterhetett_kapott = 0;
    }else if(Hazaterhet == info->si_value.sival_ptr){
        hazaterhetett_kapott = 1;
    }else{
        printf("elírtál valamit! Nem jó adat küldesz a signallal!\n");
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 1);

    char* teruletek[] = {"Barátfa", "Lovas", "Kígyós-patak", "Káposztás", "Szula", "Malom telek", "Páskom"};

    pid_t gy1 = fork();
    if(gy1){
        pid_t gy2 = fork();
        if(gy2){
            whoami = "Szülő / főnyuszi";
            signal(SIGUSR1, dummy);
            pause();
            pause();

            struct msg to_send = {.type = FO_TO_FULES};
            strcpy(to_send.buf, teruletek[rand() % 7]);
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
            
            to_send.type = FO_TO_TAPSI;
            strcpy(to_send.buf, teruletek[rand() % 7]);
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
            
            
            struct szam_msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), TAPSI_TO_FO, 0);
            int tapsi_szamlalt = to_receive.nepszam;
            int tapsi_helyen_tavaly = rand() % 51 + 50;
            int gyarapodas_tapsi = to_receive.nepszam > tapsi_helyen_tavaly;
            pt("Tapsi ennyit számolt: %d, tehát %s történt", to_receive.nepszam, (gyarapodas_tapsi ? "gyarapodás" : "fogyás"));

            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), FULES_TO_FO, 0);
            int fules_szamlalt = to_receive.nepszam;
            int fules_helyen_tavaly = rand() % 51 + 50;
            int gyarapodas_fules = to_receive.nepszam > fules_helyen_tavaly;
            pt("Füles ennyit számolt: %d, tehát %s történt", to_receive.nepszam, (gyarapodas_fules ? "gyarapodás" : "fogyás"));

            sleep(1);
            
            union sigval s_value_ptr = {.sival_ptr = (gyarapodas_tapsi ? Hazaterhet : Keress_meg)};
            sigqueue(gy2, SIGUSR1, s_value_ptr);
            s_value_ptr = (union sigval){.sival_ptr = (gyarapodas_fules ? Hazaterhet : Keress_meg)};
            sigqueue(gy1, SIGUSR1, s_value_ptr);

            if(!gyarapodas_tapsi){
                msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), TAPSI_TO_FO, 0);
                pt("Tapsi újraszámolt, ennyi lett a végeredmény: %d", to_receive.nepszam);
                tapsi_szamlalt = to_receive.nepszam;
            }
            if(!gyarapodas_fules){
                msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), FULES_TO_FO, 0);
                pt("Füles újraszámolt, ennyi lett a végeredmény: %d", to_receive.nepszam);
                fules_szamlalt = to_receive.nepszam;
            }

            struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
            char* shared = shmat(shm,NULL,0);
            //pt("tapsi_helyen_tavaly (%d) tapsi_szamlalt (%d)", tapsi_helyen_tavaly, tapsi_szamlalt);
            if(tapsi_helyen_tavaly < tapsi_szamlalt){
                strcpy(shared, "Tapsi gyarapodást tapasztalt.");
            }else{
                strcpy(shared, "Tapsi népességfogyást tapasztalt.");
            }

            //pt("fules_helyen_tavaly (%d) fules_szamlalt (%d)", fules_helyen_tavaly, fules_szamlalt);
            if(fules_helyen_tavaly < fules_szamlalt){
                strcat(shared, " Füles gyarapodást tapasztalt.");
            }else{
                strcat(shared, " Füles népességfogyást tapasztalt.");
            }
            pt("Az osztott memóriában: %s", shared);
            shmdt(shared);
            muvelet = (struct sembuf){.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);

            waitpid(gy1, 0, 0);
            waitpid(gy2, 0, 0);
            msgctl(msgQ, IPC_RMID, NULL);
            shmctl(shm, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
        }else{
            whoami = "Tapsi";
            struct sigaction sigact_usr1 = {.sa_sigaction = advanced_handler, .sa_flags = SA_SIGINFO};
            sigemptyset(&sigact_usr1.sa_mask);
            sigaction(SIGUSR1, &sigact_usr1, NULL);
            sleep(1);
            kill(getppid(), SIGUSR1);

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), FO_TO_TAPSI, 0);
            pt("Itt számolok: %s", to_receive.buf);

            struct szam_msg to_send = {.type = TAPSI_TO_FO, .nepszam = rand() % 51 + 50};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            pause();

            if(!hazaterhetett_kapott){
                pt("Keresnem kell még");
                to_send = (struct szam_msg){.type = TAPSI_TO_FO, .nepszam = rand() % 31 + 70};
                msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
            }
        }
    }else{
        whoami = "Füles";
        struct sigaction sigact_usr1 = {.sa_sigaction = advanced_handler, .sa_flags = SA_SIGINFO};
        sigemptyset(&sigact_usr1.sa_mask);
        sigaction(SIGUSR1, &sigact_usr1, NULL);
        sleep(2);
        kill(getppid(), SIGUSR1);

        struct msg to_receive;
        msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), FO_TO_FULES, 0);
        pt("Itt számolok: %s", to_receive.buf);

        struct szam_msg to_send = {.type = FULES_TO_FO, .nepszam = rand() % 51 + 50};
        msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

        pause();
        if(!hazaterhetett_kapott){
            pt("Keresnem kell még");
            to_send = (struct szam_msg){.type = FULES_TO_FO, .nepszam = rand() % 31 + 70};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);
        }
    }
    return 0;
}