#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#define ITER 10

int main(int argc, char** argv)
{
	
	int pid, i,tamano;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	
	int world_size;
  	MPI_Comm_size (MPI_COMM_WORLD,&tamano);
	
	int cont = 0;
	int pid_op = (pid + 1) % 2;
	printf("PID : %d y Pid OP %d \n", pid, pid_op);

	for(i=0; i<ITER; i++){
		printf("PID : %d\n",pid );
		if (pid == cont % 2){
			cont++;
			MPI_Send(&cont, 1, MPI_INT, pid_op, 0, MPI_COMM_WORLD);
      		printf("Proceso %d incrementa el contador=%d y envia al Proceso %d\n",pid, cont, pid_op);
		}
		else
		{
			MPI_Recv(&cont, 1, MPI_INT, pid_op, 0, MPI_COMM_WORLD,&status);
      		printf("Proceso %d recibe el contador=%d del Proceso %d\n",pid , cont, pid_op);
		}
	}	
	fflush(stdout);
	MPI_Finalize();
	return 0;
}