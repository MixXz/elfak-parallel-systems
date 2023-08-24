#include<stdio.h>
#include<math.h>
#include<mpi.h>

#define MASTER 0
#define is_diag(rank, n) (rank % (n + 1) == 0)

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int n = (int)sqrt(size);

	MPI_Comm diag_comm;
	MPI_Comm_split(MPI_COMM_WORLD, is_diag(rank, n), 0, &diag_comm);

	int diag_rank;
	MPI_Comm_rank(diag_comm, &diag_rank);

	if (is_diag(rank,n)) {
		int buff;

		if (diag_rank == MASTER) {
			buff = 1;
		}

		MPI_Bcast(&buff, 1, MPI_INT, MASTER, diag_comm);
		printf("Old rank: %d, new rank: %d, message: %d\n", rank, diag_rank, buff);
	}

	MPI_Finalize();
	return 0;
}