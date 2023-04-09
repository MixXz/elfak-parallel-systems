#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define MASTER 0
#define DEFAULT_TAG 0

//Napisati MPI program koji kreira komunikator koji se sastoji od dijagonalnih procesa u
//kvadratnoj mreži procesa.Iz master procesa novog komunikatora poslati poruku svim ostalim
//procesima.Svaki proces novog komunikatora treba da prikaže primljenu poruku, identifikator
//procesa u novom komunikatoru i stari identifikator procesa.

int main(int argc, char** argv) {
    int rank, newRank, size;
    MPI_Group oldGroup, newGroup;
    MPI_Comm newComm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = sqrt(size);
    int* members = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++) 
        members[i] = i * (n + 1);

    MPI_Comm_group(MPI_COMM_WORLD, &oldGroup);
    MPI_Group_incl(oldGroup, n, members, &newGroup);
    MPI_Comm_create(MPI_COMM_WORLD, newGroup, &newComm);

    MPI_Group_rank(newGroup, &newRank);

    if (rank % (n + 1) == 0) {
        int buff;

        if (newRank == MASTER) {
            buff = 1;
            for (int i = 1; i < n; i++)
                MPI_Send(&buff, 1, MPI_INT, i, DEFAULT_TAG, newComm);
        }
        else
            MPI_Recv(&buff, 1, MPI_INT, MASTER, DEFAULT_TAG, newComm, MPI_STATUS_IGNORE);
            
        printf("Stari rank [%d]\tnovi rank [%d].\n", rank, newRank);
    }

    free(members);
    MPI_Finalize();

    return 0;
}