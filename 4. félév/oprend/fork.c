#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t  child1 = fork(); // gyerek1 process létrehozása
    if (child1 == 0) // gyerek1
    {
        exit(0); // kilépünk a processből hogy ne fusson ugyan az a kód tovább
    }

    pid_t child2 = fork();  // gyerek2 process létrehozása
    if( child2 == 0) // gyerek 2
    {
        exit(0); // kilépünk a processből hogy ne fusson ugyan az a kód tovább
    }
    
    // szülő
    waitpid(child1,NULL,0); // Megvárja míg a gyerek1 process befejeződik
    waitpid(child2,NULL,0); // Megvárja míg a gyerek2 process befejeződik
    
    // wait(NULL) Szintén megvárja míg az egyik gyerek process befejeződik

    return 0;
}