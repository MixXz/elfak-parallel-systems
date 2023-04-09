#include <mpi.h>
#include <stdio.h>
#define N 4
#define M 3
#define DIM_COUNT 2
#define DISPLACEMENT 1
#define FIRST_DIM 0
#define SECOND_DIM 1
#define MASTER 0

//Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i m
//kolona.Svaki od nxm procesa izračunava sumu identifikatora njegovog gornjeg i donjeg
//suseda.Obezbediti da se u master procesu na kraju programa nađe ukupna izračunata suma.

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm cartComm;
	int cartRank,
		upNeighbourRank,
		downNeighbourRank,
		cartCoords[DIM_COUNT],
		dims[DIM_COUNT] = { N, M },
		periods[DIM_COUNT] = { 0, 1 },
		loc_sum, sum;

	MPI_Cart_create(MPI_COMM_WORLD, DIM_COUNT, dims, periods, 1, &cartComm);
	MPI_Comm_rank(cartComm, &cartRank);
	MPI_Cart_coords(cartComm, rank, DIM_COUNT, cartCoords);

	MPI_Cart_shift(cartComm, SECOND_DIM, DISPLACEMENT, &upNeighbourRank, &downNeighbourRank);

	loc_sum = upNeighbourRank + downNeighbourRank;

	printf("Rank: %d \tCoords(%d, %d) \tUp rank: %d \t Down rank: %d \t Sum: %d\n",
		cartRank,
		cartCoords[0],
		cartCoords[1],
		upNeighbourRank,
		downNeighbourRank,
		loc_sum);

	MPI_Reduce(&loc_sum, &sum, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER)
		printf("Sum of all: %d\n", sum);

	MPI_Finalize();
	return 0;
}