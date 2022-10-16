#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

//Este archivo tiene conexión con el archivo main.c de "04. Obtener el valor de retorno de un thread"

//Tirar dados
void* roll_dice() {
    int value = (rand() % 6) + 1;
    int* result = malloc(sizeof(int));
    *result = value; //el valor de result equivale al value
    //printf("Valor: %d\n", value);
    printf("Thread result: %d\n", *result);
    pthread_exit((void*) result);
}

int main(int argc, char const *argv[]) {
    int* result;
    srand(time(NULL));
    pthread_t th;
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }

    pthread_exit(0); //útil para iniciar algunos threads en el thread principal (main process) y queremos dejarlos ejecutándo mientras el proceso principal finaliza.

    if (pthread_join(th, (void**) &result) != 0) {
        return 2;
    }

    printf("Resultado: %d\n", *result);
    free(result);
    return 0;
}
