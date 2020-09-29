#include <stdio.h>
#include <mpi.h> //se usará mpi


// Calcula la integral local
double Trap(double left_endpt, double right_endpt, int trap_count, 
   double base_len);    

// Función que estamos integrando
double f(double x); 

int main(void) {
   int my_rank, comm_sz, n = 1024, local_n;   
   double a = 0.0, b = 3.0, h, local_a, local_b;
   double local_int, total_int;
   int source; 
   
   MPI_Init(NULL, NULL); // Iniciando mpi
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); //Obteniendo el rango de proceso
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); //Buscando procesos utilizados

   h = (b-a)/n; // calculando h        
   local_n = n/comm_sz;  // nun de trapezoides
   // Duracion del intervalo de cada proceso
   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;
   local_int = Trap(local_a, local_b, local_n, h);
   // Suma las integrales calculadas por cada proceso   
   if (my_rank != 0) { 
      MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, 
            MPI_COMM_WORLD); 
   } else {
      total_int = local_int;
      for (source = 1; source < comm_sz; source++) {
         MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         total_int += local_int;
      }
   } 
   // Imprime el resultado
   if (my_rank == 0) {
      printf("Con n = %d trapezoides, nuestro estimado\n", n);
      printf("de la integral para%f a %f = %.15e\n",
          a, b, total_int);
   }
   // Finalizando mpi
   MPI_Finalize();
   return 0;
} 
// Función serial para estimar una integral definida usando la regla trapezoidal
double Trap(
      double left_endpt, 
      double right_endpt, 
      int    trap_count, 
      double base_len ) {
   double estimate, x; 
   int i;

   estimate = (f(left_endpt) + f(right_endpt))/2.0;
   for (i = 1; i <= trap_count-1; i++) {
      x = left_endpt + i*base_len;
      estimate += f(x);
   }
   estimate = estimate*base_len;
   return estimate;
} 

double f(double x) {
   return x*x;
} 
