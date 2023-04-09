#include <mpi.h>
#include <stdio.h>
#define N 4
#define M 3
#define DIM_COUNT 2
#define MASTER 0
#define DEFAULT_TAG 0

//NISAM SIGURAN DA LI SAM DOBRO SHVATIO ZADATAK.

//Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i m
//kolona.Za svaki skup procesa koji pripadaju istoj koloni strukture kreirati novi komunikator.
//Master procesu iz svake kolone poslati koordinate procesa sa najvećim identifikatorom i
//prikazati ih.

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm cartComm, colsComm;

	int cartRank,
		colRank,
		cartCoords[DIM_COUNT],
		dims[DIM_COUNT] = { N, M },
		periods[DIM_COUNT] = { 0,1 };

	MPI_Cart_create(MPI_COMM_WORLD, DIM_COUNT, dims, periods, 1, &cartComm);
	MPI_Comm_rank(cartComm, &cartRank);
	MPI_Cart_coords(cartComm, cartRank, DIM_COUNT, cartCoords);

	MPI_Comm_split(cartComm, cartCoords[0], cartCoords[1], &colsComm);
	MPI_Comm_rank(colsComm, &colRank);

	if (colRank == M - 1)
		MPI_Send(&cartCoords, 2, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD);

	if (rank == MASTER) {
		int buff[2];
		for (int i = 0; i < N; i++) {
			MPI_Recv(&buff, 2, MPI_INT, MPI_ANY_SOURCE, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			printf("Coords(%d, %d)\n", buff[0], buff[1]);
		}
	}

	MPI_Finalize();
	return 0;
}