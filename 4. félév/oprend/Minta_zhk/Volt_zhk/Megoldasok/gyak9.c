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

struct cim_telefon{
 long type;
 char lakcim[50];
 pid_t telefon;
};
struct msg{
 long type;
 char buf[64];
};

int csak_az_utasnak_hogy_tudja_mi_a_taxis_PIDje;
void advanced_handler_utas(int signumber, siginfo_t *info, void* unused){
    csak_az_utasnak_hogy_tudja_mi_a_taxis_PIDje = info->si_pid;
}

int csak_a_taxisnak_a_kiolvasott_tavolsagertek;
void advanced_handler_taxis(int signumber, siginfo_t *info, void* unused){
    csak_a_taxisnak_a_kiolvasott_tavolsagertek = info->si_value.sival_int;
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    key_t key = ftok(argv[0],37);
    int msgQ = msgget(key, 0600 | IPC_CREAT);
    int shm = shmget(key, 100, S_IRUSR | S_IWUSR | IPC_CREAT);
    int sem = semget(key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
    semctl(sem, 0, SETVAL, 0);

    int utas_to_kozpont[2];
    pipe(utas_to_kozpont);
    pid_t gy1 = fork();
    if(gy1){
        pid_t gy2 = fork();
        if(gy2){
            whoami = "központ";
            close(utas_to_kozpont[1]);
            char buf[50];
            read(utas_to_kozpont[0], buf, sizeof(buf));
            close(utas_to_kozpont[0]);
            pt("Az utas ide akar menni: %s", buf);

            struct cim_telefon to_send = {.type = 5, .telefon = gy1};
            strcpy(to_send.lakcim, buf);
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            struct msg to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 6, 0);
            pt("Ezt mondja a taxis: %s", to_receive.buf);

            struct sembuf muvelet = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
            int* shared = shmat(shm,NULL,0);
            pt("A taxis bevétele: %d", *shared);
            shmdt(shared);

            waitpid(gy1, 0, 0);
            waitpid(gy2, 0, 0);
            msgctl(msgQ, IPC_RMID, NULL);
            shmctl(shm, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
        }else{
            whoami = "taxis";
            close(utas_to_kozpont[0]);
            close(utas_to_kozpont[1]);
            struct sigaction sigact_usr1 = {
                .sa_sigaction = advanced_handler_taxis, //nem sa_handler
                .sa_flags = SA_SIGINFO //ez már többet tud
            };
            sigemptyset(&sigact_usr1.sa_mask);
            sigaction(SIGUSR1, &sigact_usr1,NULL);
            
            struct cim_telefon to_receive;
            msgrcv(msgQ, &to_receive, sizeof(to_receive) - sizeof(long), 5, 0);
            pt("Egy utast el kell vinnem, a lakcím \"%s\", a telefonszám (PID) pedig %d", to_receive.lakcim, to_receive.telefon);
            
            struct msg to_send = {.type = 6, .buf = "Elindultam az utasért!"};
            msgsnd(msgQ, &to_send, sizeof(to_send) - sizeof(long), 0);

            sleep(1);
            pt("Megjöttem az utasért! Szólok neki!");
            kill(to_receive.telefon, SIGUSR1);
            
            pause();
            int ar = csak_a_taxisnak_a_kiolvasott_tavolsagertek * 300 + 700;
            pt("a taxi ára: %d", ar);

            int* shared = shmat(shm,NULL,0);
            *shared = ar;
            shmdt(shared);
            struct sembuf muvelet = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
            semop(sem, &muvelet, 1);
        }
    }else{
        whoami = "utas";
        struct sigaction sigact_usr1 = {
            .sa_sigaction = advanced_handler_utas, //nem sa_handler
            .sa_flags = SA_SIGINFO //ez már többet tud
        };
        sigemptyset(&sigact_usr1.sa_mask);
        sigaction(SIGUSR1, &sigact_usr1,NULL);

        close(utas_to_kozpont[0]);
        write(utas_to_kozpont[1], argv[1], strlen(argv[1]) + 1);
        close(utas_to_kozpont[1]);

        pause();
        int cim = rand() % 20 + 1;
        pt("ezt a címet sorsoltam ki: %d", cim);
        union sigval s_value_ptr = {.sival_int = cim};
        sleep(1);
        sigqueue(csak_az_utasnak_hogy_tudja_mi_a_taxis_PIDje, SIGUSR1, s_value_ptr);
    }
    return 0;
}