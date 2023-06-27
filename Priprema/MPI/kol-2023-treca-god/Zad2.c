#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define MASTER 0
#define DEFAULT_TAG 0

#define N 4
#define M 4

int main(int argc, char** argv) {
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const int ndims[] = { N, M },
		periods[] = { 1, 0 };

	MPI_Comm cartComm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, ndims, periods, 1, &cartComm);

	int coords[2], cartRank;
	MPI_Cart_coords(cartComm, rank, 2, coords);
	MPI_Cart_rank(cartComm, coords, &cartRank);

	int left, right;
	MPI_Cart_shift(cartComm, 0, 2, &left, &right);

	int k = cartRank;

	cartRank % 2 == 0
		&& printf("Process[%d] coords(%d, %d):\n", cartRank, coords[0], coords[1])
		&& printf("K pre: %d\n", k);

	MPI_Sendrecv_replace(&k, 1, MPI_INT, right, DEFAULT_TAG, left, DEFAULT_TAG, cartComm, MPI_STATUS_IGNORE);

	k += cartRank;
	cartRank % 2 == 0 && printf("K posle: %d\n", k);

	MPI_Finalize();
	return 1;
}
