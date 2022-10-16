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
    for (int i=0; i<20; i++) {
        pthread_mutex_lock(&mutexFuel);
        *fuel += 15;
        printf("Filling fuel = %d\n", *fuel);
        pthread_mutex_unlock(&mutexFuel);
        pthread_cond_broadcast(&conditionFuel); //enviará una señal a todos los thread para que revisen si ahora tienen la posibilidad de restar 40 de fuel.
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
        //Cada mensaje de No fuel... Waiting es de un thread distinto. Lo que sucede es que el thread ve que fuel es menor a 40 por lo que entra aquí y espera sobre la condición &conditionFuel. PERO PRIMERO LIBERA EL MUTEX (detrás de escenas de pthread_cond_wait) y luego se pone a esperar por señales en esa condición. Ahí es cuando otro thread lockea el mutex y se fija si fuel es menor a 40; como lo es, entra aquí e imprime el mismo mensaje de No fuel... Waiting, libera el mutex y se pone a esperar por señales en conditionFuel.
        //Entonces, ¿qué pasa cuando un thread que estaba esperando por señales, finalmente recibe una? Pues se "despierta", lockea el mutex y continúa su ejecución. Ahora bien, sólo un thread puede hacer esto si usamos pthread_cond_signal (porque sólo envía una señal). Podríamos querer que todos los threads se despierten e intenten lockear el mutex (sólo uno va a lograrlo) para continuar su ejecución, y que los threads que deben hacer waiting por el mutex sólo tengan que esperar a que el mutex se unlockee y no a recibir una señal. Para eso usamos pthread_cond_broadcast.
        //Si la señal se nos envió mediante pthread_cond_broadcast, entonces todos los threads que empezaron a esperar aquí se despertarán y continuarán ejecución.
        
        /**
         * ¿Cómo es posible que 2 coches estén en lock? ¿No es acaso el concepto de mutex que sólo un thread está en situación de lock y los otros están esperando?
         * Eso es verdad, y el truco esta con pthread_cond_wait, porque detrás de escenas hace esto:
         *  pthread_mutex_unlock;
         *  wait for signal
         *  pthread_mutex_lock;
         * Y justamente por ese pthread_mutex_unlock que pthread_cond_wait ejecuta, es que otros threads están permitidos para lockear ese mutex. Por eso debemos pasarle el mutex a pthread_cond_wait.
         */

        /**
         * Supongamos la siguiente situación: 
         *  - Hay 45 de fuel.
         *  - Un thread va a ser signaled. Dirá que hay más de 40 con lo cual sale del loop while.
         *  - Restará 40 de fuel.
         *  - Imprimirá el mensaje y unlockeará el mutex.
         *  - Fuel = 5
         *  - En este punto, no tiene sentido chequear que fuel < 40 en el while loop, porque sabemos que va a ser 5. No siempre es el caso porque en lugar de ir llenando de 15 en 15, podemos llenar de 60 en 60.
         *  - Problema nuevo: cuando tenemos 80 de fuel, dos coches podrían haber restado 40 de fuel para llenarse. Pero sólo uno lo hizo, y el thread que llena el fuel comenzó a llenar de vuelta. Porque sólo un thread se "despertó" mediante el pthread_cond_signal.
         *  - Así que hay múltiples threads esperando en la condición &condFuel, pero sólo uno de ellos es signaled para despertar.
         *  - ¿Qué podemos hacer para que todos se despierten, y de este modo, dos coches puedan llenar su tanque?
         *  - Usando pthread_cond_broadcast, que emite una señal a todos los threads que estén esperando en esa condición.
         */
    }
    *fuel -= 40;
    printf("Got fuel now left = %d\n", *fuel);
    pthread_mutex_unlock(&mutexFuel);
}

int main(int argc, char const *argv[]) {
    int* fuel = malloc(sizeof(int));
    *fuel = 0;
    pthread_t th[5];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&conditionFuel, NULL);
    for (int i=0; i<5; i++) {
        if (i == 4) {
            if (pthread_create(&th[i], NULL, &fuel_filling, fuel) != 0) {
                perror("Failed to create thread.\n");
            }
        } else {
            if (pthread_create(&th[i], NULL, &car, fuel) != 0) {
                perror("Failed to create thread.\n");
            }
        }
    }

    for (int i=0; i<5; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread.\n");
        }
    }

    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&conditionFuel);
    free(fuel);

    return 0;
}
