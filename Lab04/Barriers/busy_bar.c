#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 100

int thread_count;
int barrier_thread_counts[BARRIER_COUNT];
pthread_mutex_t barrier_mutex;

void Usage(char* prog_name);
void *Thread_work(void* rank);


int main(int argc, char* argv[]) {
   long       thread, i;
   pthread_t* thread_handles; 
   double start, finish;

   if (argc != 2)
      Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   thread_handles = malloc (thread_count*sizeof(pthread_t));
   for (i = 0; i < BARRIER_COUNT; i++)
      barrier_thread_counts[i] = 0;
   pthread_mutex_init(&barrier_mutex, NULL);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_work, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }
   GET_TIME(finish);
   printf("Tiempo transcurrido = %e segundos\n", finish - start);

   pthread_mutex_destroy(&barrier_mutex);
   free(thread_handles);
   return 0;
}  /* main */

//Imprimir línea de comando para función y terminar
void Usage(char* prog_name) {

   fprintf(stderr, "Uso: %s <numero de threads>\n", prog_name);
   printf(" ");
   exit(0);
}  


// Corre con BARRIER_COUNT
void *Thread_work(void* rank) {
#  ifdef DEBUG
   long my_rank = (long) rank; 
#  endif
   int i;

   for (i = 0; i < BARRIER_COUNT; i++) {
      pthread_mutex_lock(&barrier_mutex);
      barrier_thread_counts[i]++;
      pthread_mutex_unlock(&barrier_mutex);
      while (barrier_thread_counts[i] < thread_count);
#     ifdef DEBUG
      if (my_rank == 0) {
         printf(" Todos los hilos entraron en la barrera %d\n", i);
         fflush(stdout);
      }
#     endif
   }

   return NULL;
}  
