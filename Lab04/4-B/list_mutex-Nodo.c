#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

// Las entradas aleatorias son inferiores a MAX_KEY
const int MAX_KEY = 100000000;
//Valores de retorno de Advance_ptrs
const int IN_LIST = 1;
const int EMPTY_LIST = -1;
const int END_OF_LIST = 0;
//Estructura para nodos de lista
struct list_node_s {
   int    data;
   pthread_mutex_t mutex;
   struct list_node_s* next;
};

//Variables compartidas
struct list_node_s* head = NULL;  
pthread_mutex_t head_mutex;
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
pthread_mutex_t count_mutex;
int         member_total=0, insert_total=0, delete_total=0;

// Configuración y limpieza
void        Usage(char* prog_name);
void        Get_input(int* inserts_in_main_p);

/* Thread funcion */
void*       Thread_work(void* rank);

//Lista de operaciones
void        Init_ptrs(struct list_node_s** curr_pp, 
      struct list_node_s** pred_pp);
int         Advance_ptrs(struct list_node_s** curr_pp, 
      struct list_node_s** pred_pp);
int         Insert(int value);
void        Print(void);
int         Member(int value);
int         Delete(int value);
void        Free_list(void);
int         Is_empty(void);

int main(int argc, char* argv[]) {
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   int inserts_in_main;
   unsigned seed = 1;
   double start, finish;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);

   Get_input(&inserts_in_main);

   //Intenta insertar claves inserts_in_main, pero abandona después
   i = attempts = 0;
   pthread_mutex_init(&head_mutex, NULL);
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) i++;
   }
   printf("Insertado %ld llaves en lista vacía\n", i);

#  ifdef OUTPUT
   printf("Antes de comenzar los hilos, enumere = \n");
   Print();
   printf("\n");
#  endif

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);

   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
   printf("Tiempo transcurrido= %e segundos\n", finish - start);
   printf("Operaciones totales = %d\n", total_ops);
   printf("operaciones de miembros = %d\n", member_total);
   printf("Insertar operaciones= %d\n", insert_total);
   printf("Eliminar operaciones = %d\n", delete_total);

#  ifdef OUTPUT
   printf("Después de que terminen los hilos, lista = \n");
   Print();
   printf("\n");
#  endif

   Free_list();
   pthread_mutex_destroy(&head_mutex);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
} 

void Usage(char* prog_name) {
   fprintf(stderr, "Uso: %s <thread_count>\n", prog_name);
   exit(0);
}  

void Get_input(int* inserts_in_main_p) {

   printf("¿Cuántas keys deben insertarse en el hilo principal?\n");
   scanf("%d", inserts_in_main_p);
   printf("¿Cuántas operaciones en total deben ejecutarse?\n");
   scanf("%d", &total_ops);
   printf("¿Porcentaje de operaciones que deberían ser búsquedas?(entre 0 y 1)\n");
   scanf("%lf", &search_percent);
   printf("¿Porcentaje de operaciones que deberían ser inserciones?(entre 0 y 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
} 
//Inicializar los punteros pred y curr antes de iniciar la búsqueda realizada por Insertar o Eliminar
void Init_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   *pred_pp = NULL;
   pthread_mutex_lock(&head_mutex);
   *curr_pp = head;
   if (*curr_pp != NULL)
      pthread_mutex_lock(&((*curr_pp)->mutex));
// pthread_mutex_unlock(&head_mutex);
      
}  
// Avanzar el par de punteros pred y curr durante insertar o eliminar
int Advance_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   int rv = IN_LIST;
   struct list_node_s* curr_p = *curr_pp;
   struct list_node_s* pred_p = *pred_pp;

   if (curr_p == NULL) {
      if (pred_p == NULL) {
         pthread_mutex_unlock(&head_mutex);//encabeza la lista
         return EMPTY_LIST;
       } else { //No encabeza la lista
         return END_OF_LIST;
       }
   } else { // *curr_pp != NULL
      if (curr_p->next != NULL)
         pthread_mutex_lock(&(curr_p->next->mutex));
      else
         rv = END_OF_LIST;
      if (pred_p != NULL)
         pthread_mutex_unlock(&(pred_p->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      *pred_pp = curr_p;
      *curr_pp = curr_p->next;
      return rv;
   }
}  

// Inserte el valor en la ubicación numérica correcta en la lista
int Insert(int value) {
   struct list_node_s* curr;
   struct list_node_s* pred;
   struct list_node_s* temp;
   int rv = 1;

   Init_ptrs(&curr, &pred);
   
   while (curr != NULL && curr->data < value) {
      Advance_ptrs(&curr, &pred);
   }
   //Si el valor no está en la lista, devuelve 1, de lo contrario devuelve 0
   if (curr == NULL || curr->data > value) {
#     ifdef DEBUG
      printf("Insertando %d\n", value);
#     endif
      temp = malloc(sizeof(struct list_node_s));
      pthread_mutex_init(&(temp->mutex), NULL);
      temp->data = value;
      temp->next = curr;
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred == NULL) {
         // Insertar en el encabezado de la lista
         head = temp;
         pthread_mutex_unlock(&head_mutex);
      } else {
         pred->next = temp;
         pthread_mutex_unlock(&(pred->mutex));
      }
   } else { //value in list
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

   return rv;
} 
//No usa bloqueos: no se puede ejecutar con los otros hilos
void Print(void) {
   struct list_node_s* temp;

   printf("lista = ");

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
} 

int  Member(int value) {
   struct list_node_s *temp, *old_temp;

   pthread_mutex_lock(&head_mutex);
   temp = head;
   if (temp != NULL) pthread_mutex_lock(&(temp->mutex));
   pthread_mutex_unlock(&head_mutex);
   while (temp != NULL && temp->data < value) {
      if (temp->next != NULL) 
         pthread_mutex_lock(&(temp->next->mutex));
      old_temp = temp;
      temp = temp->next;
      pthread_mutex_unlock(&(old_temp->mutex));
   }

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d no esta en la lista\n", value);
#     endif
      if (temp != NULL) 
         pthread_mutex_unlock(&(temp->mutex));
      return 0;
   } else { /* temp != NULL && temp->data <= value */
#     ifdef DEBUG
      printf("%d esta en la lista\n", value);
#     endif
      pthread_mutex_unlock(&(temp->mutex));
      return 1;
   }
}  
//Elimina valor de la lista
int Delete(int value) {
   struct list_node_s* curr;
   struct list_node_s* pred;
   int rv = 1;

   Init_ptrs(&curr, &pred);

   //Busca valor
   while (curr != NULL && curr->data < value) {
      Advance_ptrs(&curr, &pred);
   }
   // Si el valor está en la lista, devuelve 1, de lo contrario, devuelve 0
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { //primer elemento en la lista
         head = curr->next;
#        ifdef DEBUG
         printf("Liberando %d\n", value);
#        endif
         pthread_mutex_unlock(&head_mutex);
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      } else { /* pred != NULL */
         pred->next = curr->next;
         pthread_mutex_unlock(&(pred->mutex));
#        ifdef DEBUG
         printf("Liberando %d\n", value);
#        endif
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      }
   } else { //no esta en la lista
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      if (curr != NULL)
         pthread_mutex_unlock(&(curr->mutex));
      if (curr == head)
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

   return rv;
} 
// No usa cerraduras. Solo se puede ejecutar cuando ningún otro hilo está accediendo a la lista
void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
#     ifdef DEBUG
      printf("Liberando %d\n", current->data);
#     endif
      free(current);
      current = following;
      following = current->next;
   }
#  ifdef DEBUG
   printf("Liberando %d\n", current->data);
#  endif
   free(current);
} 
int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}  
void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
#        ifdef DEBUG
         printf("Thread %ld > Buscando para %d\n", my_rank, val);
#        endif
         Member(val);
         my_member++;
      } else if (which_op < search_percent + insert_percent) {
#        ifdef DEBUG
         printf("Thread %ld >  Intentando insertar %d\n", my_rank, val);
#        endif
         Insert(val);
         my_insert++;
      } else { /* delete */
#        ifdef DEBUG
         printf("Thread %ld > Intentando borrar %d\n", my_rank, val);
#        endif
         Delete(val);
         my_delete++;
      }
   }  /* for */

   pthread_mutex_lock(&count_mutex);
   member_total += my_member;
   insert_total += my_insert;
   delete_total += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
} 
