#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* incMails() {
    printf("Test desde threads.\n");
    sleep(3);
    printf("Terminando el thread.\n");
}

int main(int argc, char const *argv[]) {
    //Creamos un thread/hilo
    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, incMails, NULL) != 0) {
        return 1;
    }
    if (pthread_create(&t2, NULL, incMails, NULL) != 0) {
        return 1;
    }

    if (pthread_join(t1, NULL) != 0) {
        return 2;
    }

    if (pthread_join(t2, NULL) != 0) {
        return 2;
    }
    return 0;
}
