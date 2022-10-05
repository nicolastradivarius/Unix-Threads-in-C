#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int x = 2; //si la variable hubiese sido declarada localmente, sería local a cada thread, y por lo tanto cada thread modificaría su propia variable x.

void* incMails() {
    /*Obtenemos el mismo PID porque en realidad ambos hilos se están ejecutando en el mismo proceso: un proceso puede tener varios hilos de ejecución, pero un hilo no puede tener varios procesos.
    Otra cuestión a tener en cuenta es el espacio de memoria: cuando forkeamos un proceso, estamos duplicando todas las variables, espacio de memoria, etc. Cuando trabajamos con threads, todos tienen acceso al mismo set de variables, comparten el espacio de direccionamiento. 
    */
    x++; //vamos a ver que su valor es igual en ambos hilos.
    sleep(2);
    //printf("Process ID: %d.\n", getpid());
    printf("Valor de x: %d\n", x);
}

void* rutina2() {
    sleep(2);
    printf("Valor de x: %d\n", x);
}

int main(int argc, char const *argv[]) {
    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, &incMails, NULL) != 0) {
        return 1;
    }
    if (pthread_create(&t2, NULL, &rutina2, NULL) != 0) {
        return 2;
    }

    if (pthread_join(t1, NULL) != 0) {
        return 1;
    }
    if (pthread_join(t2, NULL) != 0) {
        return 2;
    }

    /*Resultado:
    Valor de x: 3
    Valor de x: 3
    El valor de x es igual en ambos hilos, porque es la misma variable.
    */

    return 0;
}
