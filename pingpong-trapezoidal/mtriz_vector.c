#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
// Vector y tamano
void llenar_vec(double local_x[] ,int local_n ) {
   int i;

   for (i = 0; i < local_n; i++)
      local_x[i] = ((double) random())/((double) RAND_MAX);
}
// matriz fila = local_m y columna = n
void llenar_matriz(double local_A[] , int local_m, int n ) {
    int i, j;

    for (i = 0; i < local_m; i++)
        for (j = 0; j < n; j++) 
            local_A[i*n + j] = ((double) random())/((double) RAND_MAX);
}

void Print_vector( char title[], double local_vec[], int n ,int local_n ,int my_rank , MPI_Comm  comm ) {
    int i;
    double* vec = NULL;
    
    if (my_rank == 0) {
        vec = malloc(n*sizeof(double));
        
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, vec, local_n, MPI_DOUBLE, 0, comm);
        printf("\nThe vector %s\n", title);
        for (i = 0; i < n; i++)
            printf("%f ", vec[i]);
        
        printf("\n");
        free(vec);
    }  
    else{
        MPI_Gather(local_vec, local_n, MPI_DOUBLE, vec, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Print_matrix( char   title[],double local_A[], int m ,int local_m  ,int n ,int my_rank ,
      MPI_Comm  comm ) {
   double* A = NULL;
   int i, j, local_ok = 1;

   if (my_rank == 0) {
      A = malloc(m*n*sizeof(double));
      
      MPI_Gather(local_A, local_m*n, MPI_DOUBLE, A, local_m*n, MPI_DOUBLE, 0, comm);
      printf("\nThe matrix %s\n", title);
      for (i = 0; i < m; i++) {
         for (j = 0; j < n; j++)
            printf("%f ", A[i*n+j]);
         printf("\n");
      }
      printf("\n");
      free(A);
   } else {
      
      MPI_Gather(local_A, local_m*n, MPI_DOUBLE, A, local_m*n, MPI_DOUBLE, 0, comm);
   }
}

void mat_vect_mult(double local_A[],double local_x[],double local_y[],int local_m,int n,int	local_n,
	MPI_Comm comm) {

	double* tmp;
	tmp = (double*)malloc(n*sizeof(double));
    int local_i, j;
    // {
    // void* send_data  :   buffer de envio d datos 
    // int send_count  :    Cantidad de datos enviados
    // MPI_Datatype send_datatype : Tipo de datos enviados
    // void* recv_data  :   buffer de recepcion d datos/con parte de los datos
    // int recv_count  :    Cantidad de datos recibidos
    // MPI_Datatype recv_datatype  :    tipo de datos
    // MPI_Comm communicator :
// }
	MPI_Allgather(local_x, local_n, MPI_DOUBLE, tmp, local_n, MPI_DOUBLE, comm);

	for (local_i=0; local_i<local_m; local_i++) {
		local_y[local_i] = 0.0;
		for (j=0; j<n; j++){
            local_y[local_i] += local_A[local_i*n+j] * tmp[j];
            // printf("\n En multi[%d] : %f \n",local_i,local_y[local_i]);
        }
			
	}

	free(tmp);
}

int main() {
	int my_rank, comm_sz;

    double* local_A;
    double* local_x;
    double* local_y;
    int m,n,local_m,local_n;
    double inicio, fin ;

    m=16384;        // Fila
    n=m;        // Columna
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    local_m = m/comm_sz;
    local_n = n/comm_sz;
    
    local_A = malloc(local_m*n*sizeof(double));
    local_x = malloc(local_n*sizeof(double));
    local_y = malloc(local_m*sizeof(double));

    srandom(my_rank);
    llenar_matriz(local_A, local_m, n);
    llenar_vec(local_x, local_n);
    // Print_vector("x", local_x, n, local_n, my_rank, MPI_COMM_WORLD);
    // Print_matrix("A", local_A, m, local_m, n, my_rank, MPI_COMM_WORLD);

    // Tiempo
    inicio = MPI_Wtime();
    mat_vect_mult(local_A, local_x, local_y, local_m, n, local_n, MPI_COMM_WORLD);
    fin = MPI_Wtime();

    // Print_vector("y", local_y, m, local_m, my_rank, MPI_COMM_WORLD);

     if (my_rank == 0)
      printf("Tiempo  = %e\n", fin-inicio);

    free(local_A);
    free(local_x);
    free(local_y);
	MPI_Finalize();
	return 0;
}
