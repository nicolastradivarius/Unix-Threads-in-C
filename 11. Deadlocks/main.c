#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 8

//Definimos un struct para poder pasar más de una variable a la rutina del thread.
struct thread_arg {
    int fuel;
    int water;
} typedef thread_arg;

/**
 * Un ejemplo simple de Deadlock es cuando lockeamos un mutex dos veces.
 * ¿Qué pasa si tenemos dos mutex? Supongamos que siempre queremos drenar el agua de forma tal que siempre tengamos la misma cantidad de agua y de gasoil.
 * Los deadlocks se producen cuando tenemos múltiples locks lockeados en diferentes ordenes.
 */

pthread_mutex_t mutexFuel;
pthread_mutex_t mutexWater;

void* rutina(void* param) {
    /**
     * Este es un ejemplo que podría llevar a un deadlock. ¿Qué pasaría si un thread ejecuta la parte THEN del if, y el otro thread ejecuta la parte ELSE? Ambos threads lockean un mutex y esperan a que el otro unlockee un mutex.
     */
    if (rand() % 2 == 0) {
        pthread_mutex_lock(&mutexFuel);
        sleep(1);
        pthread_mutex_lock(&mutexWater);
    } else {
        pthread_mutex_lock(&mutexWater);
        sleep(1);
        pthread_mutex_lock(&mutexFuel);
    }

    thread_arg *arg = param;
    int fuel = (int)arg->fuel;
    int water = (int)arg->water;
    fuel += 50;
    water = fuel;
    arg->fuel = fuel;
    arg->water = water;
    printf("Incremented fuel to %d and set water to %d\n", fuel, water);

    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexWater);
}

int main(int argc, char const *argv[]) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_mutex_init(&mutexWater, NULL);

    thread_arg *arg = malloc(sizeof(*arg));
    arg->fuel = 50;
    arg->water = 10;

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &rutina, arg) != 0) {
            perror("Falló al crear el thread.\n");
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Falló al hacer join del thread.\n");
        }
    }

    pthread_mutex_destroy(&mutexFuel);
    pthread_mutex_destroy(&mutexWater);


    printf("Final fuel: %d\n", arg->fuel);
    printf("Final water: %d\n", arg->water);
    free(arg);

    return 0;
}
