#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MASTER 0
#define DEFAULT_TAG 0

#define is_diag(rank, n) (rank % (n + 1) == 0)

//Napisati MPI program koji kreira komunikator koji se sastoji od dijagonalnih procesa u
//kvadratnoj mreži procesa.Iz master procesa novog komunikatora poslati poruku svim ostalim
//procesima.Svaki proces novog komunikatora treba da prikaže primljenu poruku, identifikator
//procesa u novom komunikatoru i stari identifikator procesa.

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = (int)sqrt(size);
    int diag_rank;
    MPI_Comm diag_comm;
    
    MPI_Comm_split(MPI_COMM_WORLD, is_diag(rank, n), 0, &diag_comm);
    MPI_Comm_rank(diag_comm, &diag_rank);

    if (is_diag(rank, n)) {
        int buff; 

        if (diag_rank == MASTER) {
            buff = 1;
        }

        MPI_Bcast(&buff, 1, MPI_INT, MASTER, diag_comm);
        printf("Stari rank [%d]\tnovi rank [%d]\t poruka: %d", rank, diag_rank, buff);
    }

    MPI_Finalize();
    return 0;
}