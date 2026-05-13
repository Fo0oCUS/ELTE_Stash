#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //fork
#include <sys/wait.h> //waitpid
#include <errno.h>
#include <signal.h>
#include <time.h>

const int partokSzama = 5;

void handler(){

}

struct szavazo{
    int id;
    int ervenyes;
    int szavazat;
};

int main(int argc, char **argv){

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    int ellpid[2];
    if(pipe(ellpid) == -1){
        perror("Pipe hiba.\n");
    }
    int szurtpid[2];
    if(pipe(szurtpid) == -1){
        perror("Pipe hiba.\n");
    }
    int osszpid[2];
    if(pipe(osszpid) == -1){
        perror("Pipe hiba.\n");
    }
    int eredmenypid[2];
    if(pipe(eredmenypid) == -1){
        perror("Pipe hiba.\n");
    }

    // -----------------gyerek 1----------------
    pid_t child1 = fork();
    if(child1 < 0) {perror("Fork error.\n"); exit(1); }
    if(child1 == 0){
        close(osszpid[0]);
        close(osszpid[1]);
        close(ellpid[1]);
        close(szurtpid[0]);
        close(eredmenypid[0]);
        close(eredmenypid[1]);
        sleep(1.5);
        printf("Gyerek 1 készen áll.\n");
        kill(getppid(), SIGUSR1);

        int db;
        read(ellpid[0], &db, sizeof(int));
        printf("Ellenorzo megkapta hogy %d db szavazo erkezett.\n",db);
        struct szavazo szavazok[db];
        read(ellpid[0], szavazok, sizeof(struct szavazo) * db);
        int child2;
        read(ellpid[0], &child2, sizeof(pid_t));
        close(ellpid[0]);
        printf("Ellenorzo megkapta a szavazo id-kat.\n");
        printf("Ellenorzo kiirja azokat.\n");
        for(int i = 0; i < db; i++){
            printf("%d szavazo id: %d\n", i + 1, szavazok[i].id);
        }
        printf("Ellonorzo elkezdi az ellenorzest.\n");
        srand(time(NULL));
        for(int i = 0; i < db; i++){
            int c = rand()%5;
            if(c == 1){ szavazok[i].ervenyes = 0; }
        }
        printf("Ellenorzo kigyujtotte az ervenytelen szavazokat.\n");
        write(szurtpid[1], &db, sizeof(int));
        write(szurtpid[1], szavazok, sizeof(struct szavazo) * db);
        close(szurtpid[1]);
        sleep(1); 
        printf("Ellenorzo elkulde a jelzest ervenyesitonek nek.\n");
        kill(child2, SIGUSR2);


        exit(0);
    }

    // -----------------gyerek 2----------------
    pid_t child2 = fork();
    if(child2 < 0) {perror("Fork error.\n"); exit(1); }
    if(child2 == 0){
        close(ellpid[0]);
        close(ellpid[1]);
        close(szurtpid[1]);
        close(osszpid[0]);
        close(eredmenypid[0]);
        sleep(2);
        printf("Gyerek 2 készen áll.\n");
        kill(getppid(), SIGUSR1);

        pause();
        printf("Ervenyesito megkapta a jelzest ellenorzotol.\n");
        int db;
        read(szurtpid[0], &db, sizeof(int));
        struct szavazo szavazok[db];
        read(szurtpid[0], szavazok, sizeof(struct szavazo) * db);
        close(szurtpid[0]);
        printf("Ervenyestio kiirja a kapott szavazok adatait.\n");
        int ervenyesCount = 0;
        int ervenytelenCount = 0;
        for(int i = 0; i < db; i++){
            printf("%d szavazo id: %d\tszavazhat: %d\n", i + 1, szavazok[i].id, szavazok[i].ervenyes);
            if(szavazok[i].ervenyes == 1){
                ervenyesCount++;
            }else{ ervenytelenCount++; }
        }

        write(osszpid[1], &ervenyesCount, sizeof(int));
        write(osszpid[1], &ervenytelenCount, sizeof(int));
        close(osszpid[1]);
        srand(time(NULL));
        for(int i = 0; i < db; i++){
            if(szavazok[i].ervenyes == 1){
                szavazok[i].szavazat = rand()% partokSzama;
            }
        }

        write(eredmenypid[1], szavazok, sizeof(struct szavazo) * db);
        close(eredmenypid[1]);
        sleep(1);
        kill(getppid(), SIGUSR1);

        exit(0);
    }

    // -------------------szulo------------------

    close(ellpid[0]);
    close(szurtpid[0]);
    close(szurtpid[1]);
    close(osszpid[1]);
    close(eredmenypid[1]);
    pause();
    pause();
    printf("Szulo bevarta a gyerekeket\n");

    int db = atoi(argv[1]);
    printf("%d db szavazó érkezett.\n", db);

    struct szavazo szavazok[db];
    srand(time(NULL));
    for(int i= 0; i < db; i++){
        szavazok[i].id = rand() % 1000;
        szavazok[i].ervenyes = 1;
    }
    write(ellpid[1], &db, sizeof(int));
    write(ellpid[1], szavazok, sizeof(struct szavazo) * db);
    write(ellpid[1], &child2, sizeof(pid_t));
    close(ellpid[1]);


    int ervenyesCount;
    int ervenytelenCount;
    read(osszpid[0], &ervenyesCount, sizeof(int));
    read(osszpid[0], &ervenytelenCount, sizeof(int));
    close(osszpid[0]);
    FILE *file = fopen("adatok.txt", "w");
    fprintf(file, "ervenyesek aranya: %.0f%%\n", (float)ervenyesCount / (float)db * 100);
    fprintf(file, "ervenytelenek aranya: %.0f%%", (float)ervenytelenCount / (float)db * 100);
    fclose(file);

    printf("ervenyesek aranya: %.0f%%\n", (float)ervenyesCount / (float)db * 100);
    printf("ervenytelenek aranya: %.0f%%\n", (float)ervenytelenCount / (float)db * 100);

    pause();
    read(eredmenypid[0], szavazok, sizeof(struct szavazo) * db);
    close(eredmenypid[0]);
    for(int i = 0; i < partokSzama; i++){
        int c = 0;
        for(int j = 0; j < db; j++){
            if(szavazok[j].szavazat == i){
                c++;
            }
        }
        printf("%d. pártra %d db szavazat erkezett.\n", i + 1, c);
    }

    waitpid(child1, NULL, 0); // gyerek 1 bevárása
    waitpid(child2, NULL, 0); // gyerek 2 bevárása
    return 0;
}
