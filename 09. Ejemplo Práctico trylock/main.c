#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

/**
 * chefs = threads
 * stove = shared data (+mutex)
 */

pthread_t* th;
pthread_mutex_t* stoveMutex;


void* routine(void* arg) {
    int* stoveFuel = (int*)arg;
    
    for (int i=0; i<4; i++) {
        if (pthread_mutex_trylock(&stoveMutex[i]) == 0) {
            int fuelNeeded = (rand() % 20);
            if (stoveFuel[i] - fuelNeeded < 0) {
                printf("No more fuel... going home\n");
            } else {
                stoveFuel[i] -= fuelNeeded;
                usleep(500000);
                printf("Fuel left %d\n", stoveFuel[i]);
            }
            pthread_mutex_unlock(&stoveMutex[i]);
            break;
        } else {
            //si falla al lockear
            //si estamos en la ultima iteracion
            if (i == 3) {
                printf("No stove available yet, waiting...\n");
                usleep(300000);
                i = -1;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    int* stoveFuel = malloc(sizeof(int)*4);
    stoveFuel[0] = 100;
    stoveFuel[1] = 100;
    stoveFuel[2] = 100;
    stoveFuel[3] = 100;

    th = (pthread_t*) malloc(10*sizeof(pthread_t));
    stoveMutex = (pthread_mutex_t*) malloc(4*sizeof(pthread_mutex_t));

    for (int i=0; i<4; i++) {
        pthread_mutex_init(&stoveMutex[i], NULL);
    }
    
    for (int i=0; i<10; i++) {
        if (pthread_create(&th[i], NULL, &routine, stoveFuel) != 0) {
            perror("Error at creating thread.\n");
        }
    }

    for (int i=0; i<10; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Error at joining thread.\n");
        }
    }

    for (int i=0; i<4; i++) {
        pthread_mutex_destroy(&stoveMutex[i]);
    }
    free(stoveFuel);
    free(th);
    return 0;
}
