#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/**
 * Problema: estación de gas, tenemos alguien que está esperando para llenar su auto con gas, y otra persona está llenando el gas del auto. Usaremos 2 threads.
 */

/**
 * Una condición de variable es como un identificador para una cierta señal que puedo enviar o esperar sobre ella.
 */

/**
 * Dato adicional: en lugar de usar fuel como variable global (como hecho en el video), se utiliza una variable definida en main mediante alocado dinámico de memoria (malloc), que se envía como argumento a las rutinas que los threads ejecutan. Obviamente esta variable es manipulada en la sección crítica dado que es compartida por los dos threads que creamos, y por eso obtenemos los mismos valores que en el video. Tener en cuenta que la variable se debe des-referenciar a la hora de manipularla, y se debe pasar como puntero como argumento.
 */

pthread_mutex_t mutexFuel;
pthread_cond_t conditionFuel;

void* fuel_filling(void* arg) {
    int *fuel = (int*) arg;
    for (int i=0; i<5; i++) {
        pthread_mutex_lock(&mutexFuel);
        *fuel += 15;
        printf("Filling fuel = %d\n", *fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_signal(&conditionFuel); //enviará una señal al otro thread
        sleep(1);
    }
}

void* car(void* arg) {
    int *fuel = (int*) arg;
    pthread_mutex_lock(&mutexFuel);
    while (*fuel < 40) {
        //Es importante un bucle while porque vamos sumando el fuel de 15 en 15, y mientras vamos enviando las señales... con lo cual hay que asegurarse que fuel superó efectivamente el valor 40 (lo que hace el while) para que este thread proceda.
        printf("No fuel... Waiting\n");
        pthread_cond_wait(&conditionFuel, &mutexFuel); //esperará por una señal de otro thread. Cuando el otro thread le envía una señal, éste se "despierta" y continúa ejecución, que básicamente es revisar de nuevo la condición del while, y si es verdadera vuelve a entrar y vuelve a esperar por señales de otro thread. Con la señal enviada lo único que estamos diciendo NO es que la condición del while es true, sino que el resultado de la condición podría haber cambiadoEsto es equivalente a:
        // pthread_mutex_unlock(&mutexFuel);
        // wait for signal on condFuel
        // pthread_mutex_lock(&mutexFuel);
    }
    *fuel -= 40;
    printf("Got fuel now left = %d\n", *fuel);
    pthread_mutex_unlock(&mutexFuel);
}

int main(int argc, char const *argv[]) {
    int* fuel = malloc(sizeof(int));
    *fuel = 0;
    pthread_t th[2];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&conditionFuel, NULL);
    for (int i=0; i<2; i++) {
        if (i == 1) {
            if (pthread_create(&th[i], NULL, &fuel_filling, fuel) != 0) {
                perror("Failed to create thread.\n");
            }
        } else {
            if (pthread_create(&th[i], NULL, &car, fuel) != 0) {
                perror("Failed to create thread.\n");
            }
        }
    }

    for (int i=0; i<2; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread.\n");
        }
    }

    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&conditionFuel);
    free(fuel);

    return 0;
}
