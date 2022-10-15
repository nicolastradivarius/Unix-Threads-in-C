#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t mutex;

void* routine(void* arg) {
    //Los cuatro threads van a acceder a esta rutina, pero sólo uno va a poder lockear el mutex; el resto va a tener que esperar. Ese thread que lockeó, unlockeará el mutex, y luego algún otro thread de los que estaba esperando podrá continuar; y así siguiendo...
    // pthread_mutex_lock(&mutex);
    //Con trylock, un thread intentará lockear un mutex (no está garantizado que lo logre). En caso que no lo logre, no ejecutará el código en la sección crítica. Debemos siempre chequear el valor de retorno: es 0 si lockeamos, EBUSY (16) si no lo logra.
    //La diferencia entonces entre lock y trylock es que lock estará siempre esperando hasta que el mutex esté unlockeado, y trylock será instantáneo porque en caso de que el mutex esté lockeado, en lugar de esperar, directamente retornará el valor EBUSY.
    if (pthread_mutex_trylock(&mutex) == 0) {
        printf("Got lock!\n");
        sleep(1);
        pthread_mutex_unlock(&mutex);
    } else {
        printf("Didn't get lock.\n");
    }
}

int main(int argc, char const *argv[]) {
    pthread_t th[4];
    pthread_mutex_init(&mutex, NULL);
    for (int i=0; i<4; i++) {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) {
            perror("Error at creating thread.\n");
        }
    }

    for (int i=0; i<4; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Error at joining thread.\n");
        }
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
