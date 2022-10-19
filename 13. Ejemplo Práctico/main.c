#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 16

sem_t semaphore;

//Simulamos una cola de login para un servidor de un juego que sólo admite 12 personas adentro.
void* routine(void* args) {
    printf("(%d) Esperando en la cola de login.\n", *(int*)args);
    sem_wait(&semaphore); 
    printf("(%d) Logged in.\n", *(int*)args);
    sleep(rand() % 5 + 1);
    printf("(%d) Logged out.\n", *(int*)args);
    sem_post(&semaphore); //liberamos un puesto en la cola.
    free(args);
}

int main(int argc, char const *argv[]) {
    pthread_t th[THREAD_NUM];
    int i;
    
    /**
     * Si ponemos 32, veremos que todos los usuarios (16) pueden logearse instantáneamente y ninguno tendrá que esperar.
     * Si ponemos 4, sólo 4 usuarios podrán loguearse, y habrá una mayor cantidad de usuarios esperando.
     */
    sem_init(&semaphore, 0, 4);

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
