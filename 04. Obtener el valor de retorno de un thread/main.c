#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

//Tirar dados
void* roll_dice() {
    int value = (rand() % 6) + 1;
    int* result = malloc(sizeof(int));
    *result = value; //el valor de result equivale al value
    //printf("Valor: %d\n", value);
    printf("Thread result: %p\n", result);
    return (void*) result;
}

int main(int argc, char const *argv[]) {
    int* result;
    srand(time(NULL));
    pthread_t th;
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0) {
        return 1;
    }

    if (pthread_join(th, (void**) &result) != 0) {
        return 2;
    }

    printf("Main result: %p\n", result);
    printf("Resultado: %d\n", *result);
    free(result);
    return 0;
}
