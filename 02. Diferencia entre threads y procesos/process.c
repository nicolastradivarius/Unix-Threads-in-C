#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char const *argv[]) {
    int x = 2; //si x hubiese sido declarada global, igualmente hubiéramos tenido el mismo problema (valores de x distintos en cada proceso), porque los procesos no comparten memoria, ni siquiera la memoria global.
    int pid = fork();

    if (pid == -1) { return 1; }

    if (pid == 0) {
        x++; //se incrementa sólo en el proceso hijo.
    }

    sleep(2);
    printf("Valor de x: %d\n", x);

    //printf("Process ID: %d\n", getpid());
    if (pid != 0) {
        wait(NULL);
    }
    return 0;

    /*Resultado:
    Valor de x: 2
    Valor de x: 3
    */
}
