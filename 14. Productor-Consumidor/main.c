#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 2

/**
 * Problema del productor y el consumidor:
 */

pthread_mutex_t mutexBuffer;

int buffer[10];
int count = 0;

void* productor(void* args) {
    while (1) {
        //Genera un número aleatorio.
        int x = rand() % 100;
        pthread_mutex_lock(&mutexBuffer);
        //Añadir un elemento al buffer
        if (count < 10) {
            buffer[count] = x;
            count++;
        } else {
            /**
             * Problema: estamos salteando muchos números. Imaginemos que éstos representan requests a un servidor. Estaríamos ignorando muchas de ellas, cuando hay que atenderlas.
             * Una solución es usar dos semáforos: uno que almacena el número de slots vacíos en el buffer, y el otro que almacena el número de slots full en el buffer.
             */
            printf("Skipped %d\n", x);
        }
        pthread_mutex_unlock(&mutexBuffer);
    }
}

//Debemos usar un mutex para evitar los race conditions cuando un thread incremente el valor del contador y el otro thread lo decremente al mismo tiempo.

void* consumidor(void* args) {
    while (1) {
        int y = -1;
        //Remover un elemento del buffer
        pthread_mutex_lock(&mutexBuffer);
        if (count > 0) {
            y = buffer[count - 1];
            count--;
        }
        pthread_mutex_unlock(&mutexBuffer);
        //Consumir
        printf("Got %d\n", y);
    }
}

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexBuffer, NULL);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        //La mitad van a ser productores y la otra mitad consumidores
        if (i % 2 == 0) {
            if (pthread_create(&th[i], NULL, &productor, NULL) != 0) {
                perror("Fallo al crear el thread.\n");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consumidor, NULL) != 0) {
                perror("Fallo al crear el thread.\n");
            }
        }
    }

    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Fallo al hacer el join de los threads.\n");
        }
    }

    pthread_mutex_destroy(&mutexBuffer);

    return 0;
}
