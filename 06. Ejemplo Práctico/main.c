#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int primos[10] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };

/**
 * Problema: queremos sumar todos los números en el arreglo. La mitad va para un thread, y la mitad para otro. Luego el main process sumará las dos partes (2 threads en total).
 */

//El argumento que le pasamos equivale al cuarto parámetro de la función pthread_create
void* rutina(void* arg) {
    int sum = 0;
    int index = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        sum += primos[index + i];
    }
    printf("Suma local: %d\n", sum);
    *(int*)arg = sum;
    return arg;
}

int main(int argc, char const *argv[]) {
    pthread_t th[2];

    for (int i = 0; i < 2; i++) {
        //En cada iteración, estamos alocando 4 bytes para cada thread
        int* a = malloc(sizeof(int));
        *a = i * 5; //si i=0 esto da 0; si i=1 esto da 5, que es donde empieza la 2a parte del array.
        if (pthread_create(&th[i], NULL, &rutina, a) != 0) {
            perror("Falló al crear el thread.\n");
        }
    }

    int sumaGlobal = 0;
    int* result;

    for (int i = 0; i < 2; i++) {
        //El segundo parámetro es donde se almacena el valor de retorno de la función que el thread llamado ejecutó (arg).
        if (pthread_join(th[i], (void**) &result) != 0) {
            perror("Falló al hacer join del thread.\n");
        }
        sumaGlobal += *result;
        free(result);
    }
    printf("Suma global: %d\n", sumaGlobal);
    return 0;
}
