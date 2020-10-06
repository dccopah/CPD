#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>     //sleep
#include <math.h>
#include "timer.h"

long limite;
long long n;
double sum;
int flag;

void* Thread_sum(void* rank) {
    long my_rank = (long) rank;
    double factor;
    long long i;
    long long my_n = n/limite;
    long long my_first_i = my_n*my_rank;
    long long my_last_i = my_first_i + my_n;
    double all_sum = 0.0;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
        all_sum += factor/(2*i+1);
        
    }
    while (flag != my_rank);
    sum += all_sum;
    flag = (flag+1) % limite;

   return NULL;
}

double Serial_pi(long long n) {
   double sum = 0.0;
   long long i;
   double factor = 1.0;

   for (i = 0; i < n; i++, factor = -factor) {
      sum += factor/(2*i+1);
   }
   return 4.0*sum;

}
int main(int argc, char * argv[]) {
    long i;
    double inicio, fin ;
    pthread_t my_hilo;

    // printf("Argument: %d \n",argc);
    printf("\nSe crearan %s hilos\n",argv[1]);
    limite= atoi(argv[1]);
    n = strtoll(argv[2], NULL, 10);

    // printf("limitee : %ld \n",limite);
    
    // my_hilo = (pthread_t*) malloc (limite*sizeof(pthread_t)); 
    sum = 0.0;
    flag = 0;
    GET_TIME(inicio);
    // Create (ID threat,atributos del hilo, * d la func a ejecutar,* pasa un parametro)
    for(i=0; i < limite ; i++){
        pthread_create(&my_hilo,NULL,Thread_sum,(void*)i);
        // sleep(1);
    }

    // join(Id hilo del thread a esperar, valor de terminación del hilo)
    for (i=0; i < limite ; i++) 
        pthread_join(my_hilo, NULL);
    GET_TIME(fin);

    sum = 4.0*sum;
    printf("Con n = %lld terminos,\n", n);
    printf("Valor estimado-thr pi = %.15f\n", sum);
    printf("\n    Tiempo formula  = %e\n", fin-inicio);

    // GET_TIME(inicio);
    sum = Serial_pi(n);
    // GET_TIME(fin);

    printf("\nValor estimado serial = %.15f\n", sum);
    printf("                   pi = %.15f\n", 4.0*atan(1.0));
    // printf("\n     Tiempo serial  = %e\n", fin-inicio);
    
    // free(my_hilo);
}