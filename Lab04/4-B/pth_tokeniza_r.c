#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX = 1000;
int limite;
sem_t* semaforo;

void *tokenizar(void* rank) {
    long my_rank = (long) rank;
    int count;
    int next = (my_rank + 1) % limite;
    char *fg_rv;
    char my_line[MAX];
    char *my_string;
    char *saveptr;

    
    sem_wait(&semaforo[my_rank]);  
    fg_rv = fgets(my_line, MAX, stdin);
    sem_post(&semaforo[next]);  
    while (fg_rv != NULL) {
        printf("Thread %ld - texto = %s", my_rank, my_line);

        count = 0; 
        my_string = strtok_r(my_line, " \t\n", &saveptr);
        while ( my_string != NULL ) {
            count++;
            printf("Thread %ld > string %d = %s\n", my_rank, count, my_string);
            my_string = strtok_r(NULL, " \t\n", &saveptr);
        } 

        sem_wait(&semaforo[my_rank]); 
        fg_rv = fgets(my_line, MAX, stdin);
        sem_post(&semaforo[next]);  
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    long        hilo;
    pthread_t* threads; 

    limite = atoi(argv[1]);

    threads = (pthread_t*) malloc (limite*sizeof(pthread_t));
    semaforo = (sem_t*) malloc(limite*sizeof(sem_t));
    
    sem_init(&semaforo[0], 0, 1);
    for (hilo = 1; hilo < limite; hilo++)
        sem_init(&semaforo[hilo], 0, 0);

    printf("Texto varias veces :\n");
    for (hilo = 0; hilo < limite; hilo++)
        pthread_create(&threads[hilo],NULL, tokenizar, (void*) hilo);

    for (hilo = 0; hilo < limite; hilo++)
        pthread_join(threads[hilo], NULL);

    for (hilo=0; hilo < limite; hilo++)
        sem_destroy(&semaforo[hilo]);

    free(semaforo);
    free(threads);
    return 0;
}