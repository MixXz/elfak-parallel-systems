#include <mpi.h>
#include <stdio.h>
#define N 4
#define DIM_COUNT 2
#define MASTER 0
#define DEFAULT_TAG 0

//Napisati MPI program kojim se kreira dvodimenzionalna Cartesian struktura sa n vrsta i n
//kolona.Podeliti procese u grupe koje odgovaraju gornjoj i donjoj trougaonoj matrici kreirane
//strukture.Procese na dijagonali proizvoljno dodeliti jednoj od grupa.U okviru svake grupe
//sumirati vrednosti identifikatora svih procesa koji pripadaju datoj grupi.Master procesu
//komunikatora MPI_COMM_WORLD dostaviti ove vrednosti i odštampati ih.

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm cartComm, splitedComm;
	int cartRank,
		splitedCommRank,
		dims[DIM_COUNT] = { N, N },
		periods[DIM_COUNT] = { 0 , 1 },
		cartCoords[DIM_COUNT];

	MPI_Cart_create(MPI_COMM_WORLD, DIM_COUNT, dims, periods, 1, &cartComm);
	MPI_Comm_rank(cartComm, &cartRank);
	MPI_Cart_coords(cartComm, cartRank, DIM_COUNT, cartCoords);

	MPI_Comm_split(cartComm, cartCoords[0] >= cartCoords[1], cartRank, &splitedComm);
	MPI_Comm_rank(splitedComm, &splitedCommRank);

	int sum, lowerSum, upperAndDiagonalSum;
	MPI_Reduce(&cartRank, &sum, 1, MPI_INT, MPI_SUM, MASTER, splitedComm);

	//Kad se razdvoji sa split nastaje komunikator sa 2 podkomunikatora, svaki podkomunikator krece sa obelezavanjem
	//ranka od nule, s tim u vezi REDUCE ce za svaku podgrupu da sabere rankove i smesti u proces sa rankom 0 podgrupe.
	//E sad, nastaje problem kad je nulti proces, odnosno master proces world komunikatora, zapravo jedan od master procesa
	//unutar jedne od podgrupa. iz tog razloga ne mogu oba mastera podgrupe da posalju glavnom masteru jer je on zapravo jedan od njih 
	//i onda dolazi do deadlocka kad on pokusa sam sebi da dostavi sumu. Zato samo master podgrupe (koji nije master world komunikatora)
	//salje masteru world komunikatora, a suma drugog mastera podgrupe (odnosno mastera world comm-a :D) se samo prepakuje u promenljivu.

	if (splitedCommRank == MASTER && rank != MASTER)
		MPI_Send(&sum, 1, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD);
	else
		upperAndDiagonalSum = sum;

	if (rank == MASTER) {
		MPI_Recv(&lowerSum, 1, MPI_INT, MPI_ANY_SOURCE, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("\nLower sum: %d  Upper and diagonal sum: %d\n\n", lowerSum, upperAndDiagonalSum);
	}

	printf("Rank: %d \tCoords(%d, %d)\n", cartRank, cartCoords[0], cartCoords[1]);

	MPI_Finalize();
	return 0;
}