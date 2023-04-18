#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>
#define MASTER 0
#define DEFAULT_TAG 0

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Group diagonalGroup, worldGroup;
	MPI_Comm diagonalComm;

	const int n = (int)sqrt(size);
	int* members = calloc(n, sizeof(int));

	for (int i = 0; i < n; i++)
		members[i] = i * (n + 1);

	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_incl(worldGroup, n, members, &diagonalGroup);
	MPI_Comm_create(MPI_COMM_WORLD, diagonalGroup, &diagonalComm);

	int diagonalCommRank;
	MPI_Group_rank(diagonalGroup, &diagonalCommRank);

	if (rank % (n + 1) == 0) {
		int buff = diagonalCommRank == MASTER;
		MPI_Bcast(&buff, 1, MPI_INT, MASTER, diagonalComm);

		printf("WorldRank: %d \tDiagonalRank: %d \tReceivedMessage: %d", rank, diagonalCommRank, buff);
	}

	free(members);
	MPI_Finalize();
	return 0;
}