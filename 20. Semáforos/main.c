#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 4

sem_t semaphore;

void* routine(void* args) {
    sem_wait(&semaphore); //chequea el valor del semáforo. Si el valor del semáforo es 0, y no puede ser decrementado, entonces el thread va a esperar en ese semáforo. Si es mayor a cero (por ejemplo, 5), entonces lo decrementará y no esperará, sino que empezará a ejecutar.
    sleep(1);
    printf("Hello from thread %d\n", *(int*)args);
    sem_post(&semaphore); //hace el "reverso": en lugar de decrementar, por ejemplo de 5 a 4, lo incrementa de 4 a 5 una vez que termina la ejecución, y no espera ni nada.
    free(args);
}

int main(int argc, char const *argv[]) {
    pthread_t th[THREAD_NUM];
    int i;
    //si tenemos múltiples threads en un solo proceso, pasamos un 0 como segundo parámetro.
    //si tenemos múltiples procesos con múltiples threads, pasamos un 1.
    //El tercer parámetro es el valor inicial del parámetro.
    sem_init(&semaphore, 0, 1);

    for (i = 0; i < THREAD_NUM; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &routine, a) != 0) {
            perror("Fallo al crear thread.\n");
        }
    }

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Fallo al unir threads.\n");
        }
    }

    sem_destroy(&semaphore);

    return 0;
}
