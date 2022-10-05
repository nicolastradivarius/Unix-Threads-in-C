#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
Supongamos que tenemos un mailbox (casilla), y tenemos dos "listeners" que todo lo que hacen es incrementar el número de mails. Esos dos listeners son threads
*/
int mails = 0;
//int lock = 0;
pthread_mutex_t mutex;

void* incMails() {
    for (int i = 0; i < 100000; i++) {
        //Si el lock es 1 antes de querer setearlo en 1, entonces debemos "esperar hasta que el lock sea 0".
        //De esta forma, lo que sucede es que si un thread intenta incrementar la variable "mails", la incrementará pero también seteará el lock en 1, con lo cual si un segundo thread viene al mismo tiempo e intenta incrementarla se detendrá en esa condición porque el lock es 1. Así que solamente cuando el primer thread termine de incrementarlo, seteará lock en 0 así cuando el segundo thread viene lo lee, lo incrementa y lo escribe en memoria en el orden correcto. La API de POSIX ya tiene algo así implementado que es mucho más seguro que implementarlo nosotros (los mutex).
        // if (lock == 1) {
            
        // }
        // lock = 1;
        pthread_mutex_lock(&mutex); //es como una especie de protección ante otros threads intentando ejecutar esto al mismo tiempo.
        mails++; //Si en algún momento un thread está ejecutando esta línea de código, no habrá ningun otro thread ejecutando esta misma linea de codigo simplemente porque hay un mutex alrededor de ella.
        pthread_mutex_unlock(&mutex);
        // lock = 0;

        // leer mails
        // incrementar el valor
        // escribir mails a memoria
        // Esto está perfecto para un thread, pero cuando introducimos multiples threads, pasan cosas... En lugar de obtener el resultado deseado (que un thread incremente mails 100000 veces, y el otro lo incremente otras 100000, dando como resultado 200000 mails), obtenemos otro resultado cualquiera. Esto sucede porque puede ocurrir que un thread tome un valor de mails y el sistema operativo le dé un "break" (lo detiene), y le da lugar al otro thread para que incremente los mails. Ese otro thread termina con un valor de mails de, por ejemplo, 30, y cuando el primer thread es reanudado por el sistema operativo, éste continúa su ejecución con el valor que tomó primero antes de ser detenido, que es otro valor completamente distinto (digamos 23). Entonces este thread incrementa en 1 su valor de mails (23 + 1 = 24) y lo guarda en memoria: ahora tenemos que el valor de mails es 24 en lugar de 30 como el otro thread había calculado.
    }
}
/*Race condition ocurre sólo en dos casos:
    1) Tenemos un thread escribiendo un dato mientras otros threads lo están leyendo.
    2) Tenemos múltiples threads escribiendo el mismo dato.
Dos threads leyendo el mismo dato no causará una condición de carrera.

Para solucionar este problema, usamos un mutex: es una especie de lock alrededor de una sección del código.
*/

int main(int argc, char const *argv[]) {
    //Creamos un thread/hilo
    pthread_t t1, t2, t3, t4;
    pthread_mutex_init(&mutex, NULL);
    if (pthread_create(&t1, NULL, incMails, NULL) != 0) { return 1; }
    if (pthread_create(&t2, NULL, incMails, NULL) != 0) { return 1; }
    if (pthread_create(&t3, NULL, incMails, NULL) != 0) { return 1; }
    if (pthread_create(&t4, NULL, incMails, NULL) != 0) { return 1; }

    if (pthread_join(t1, NULL) != 0) { return 2; }
    if (pthread_join(t2, NULL) != 0) { return 2; }
    if (pthread_join(t3, NULL) != 0) { return 2; }
    if (pthread_join(t4, NULL) != 0) { return 2; }

    pthread_mutex_destroy(&mutex);

    printf("Número de mails: %d\n", mails);

    return 0;
}
