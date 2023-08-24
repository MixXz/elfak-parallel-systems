#include <stdio.h>
#include <mpi.h>
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
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const int dims[] = { N, M },
		periods[] = { 1, 0 };

	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, DIM_COUNT, dims, periods, 1, &cart_comm);

	int cart_rank, coords[2];
	MPI_Comm_rank(cart_comm, &cart_rank);
	MPI_Cart_coords(cart_comm, rank, DIM_COUNT, coords);

	int ln, rn;
	MPI_Cart_shift(cart_comm, FIRST_DIM, DISPLACEMENT, &ln, &rn);

	printf("Rank: %d \tCoords(%d, %d) \t LN rank: %d \t RN rank: %d\n", cart_rank, coords[0], coords[1], ln, rn);

	MPI_Finalize();
	return 0;
}