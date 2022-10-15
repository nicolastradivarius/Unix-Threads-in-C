#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int primos[10] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };

/**
 * Problema: Crear 10 threads, cada uno toma un número primo único del arreglo y lo imprime por pantalla.
 */

//El argumento que le pasamos equivale al cuarto parámetro de la función pthread_create
void* rutina(void* arg) {
    sleep(1);
    int index = *(int*)arg;
    printf("%d ", primos[index]);
    free(arg);
}

int main(int argc, char const *argv[]) {
    pthread_t th[10];

    for (int i = 0; i < 10; i++) {
        //En cada iteración, estamos alocando 4 bytes para cada thread
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &rutina, a) != 0) {
            perror("Falló al crear el thread.\n");
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Falló al hacer join del thread.\n");
        }
    }
    return 0;
}
