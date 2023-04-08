#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define N 4
#define M 3
#define DIM_COUNT 2
#define DISPLACEMENT 2
#define FIRST_DIM 0
#define SECOND_DIM 1

//Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i m
//kolona.U svakom od nxm procesa odštampati identifikatore procesa njegovog levog i desnog
//suseda na udaljenosti 2. Smatrati da su procesi u prvoj i poslednjoj koloni jedne vrste susedni.

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm cartComm;
	int cartRank,
		dims[DIM_COUNT] = { N, M },
		periods[DIM_COUNT] = { 0 , 1 },
		cartCoords[DIM_COUNT],
		leftNeighbourRank,
		rightNeighbourRank;

	MPI_Cart_create(MPI_COMM_WORLD, DIM_COUNT, dims, periods, 1, &cartComm);
	MPI_Comm_rank(cartComm, &cartRank);
	MPI_Cart_coords(cartComm, rank, DIM_COUNT, cartCoords);

	MPI_Cart_shift(cartComm, FIRST_DIM, DISPLACEMENT, &leftNeighbourRank, &rightNeighbourRank);

	printf("Rank: %d \tCoords(%d, %d) \t LN rank: %d \t RN rank: %d\n", cartRank, cartCoords[0], cartCoords[1], leftNeighbourRank, rightNeighbourRank);

	MPI_Finalize();
	return 0;
}