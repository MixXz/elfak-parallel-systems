#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>

#define MASTER 0

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int n = (int)sqrt(size);
	int* members = (int*)malloc(n * sizeof(int));

	for (int i = 0; i < n; i++) {
		members[i] = (n + 1) * i;
	}

	MPI_Group oldGroup, newGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &oldGroup);
	MPI_Group_incl(oldGroup, n, members, &newGroup);

	MPI_Comm diagComm;
	MPI_Comm_create(MPI_COMM_WORLD, newGroup, &diagComm);

	int newRank;
	MPI_Group_rank(newGroup, &newRank);


	if (rank % (n + 1) == 0) {
		int buff;

		if (newRank == MASTER) {
			buff = 1;
		}

		MPI_Bcast(&buff, 1, MPI_INT, MASTER, diagComm);
		printf("Old rank: %d, new rank: %d, message: %d\n", rank, newRank, buff);
	}

	free(members);
	MPI_Finalize();
	return 0;
}