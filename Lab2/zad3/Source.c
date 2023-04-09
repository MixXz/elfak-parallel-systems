#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define MASTER 0
#define V 100

//Napisati MPI program koji kreira komunikator comm1 koji se sastoji od svih procesa sa
//identifikatorima deljivim sa 3. Master proces(P0) svim procesima ove grupe šalje po jednu
//vrstu matrice A.Odštampati identifikatore procesa koji pripadaju comm1 i čija je suma
//elemenata primljene vrste matrice A manja od zadate vrednosti v.

int main(int argc, char** argv) {
	int rank, size;
	MPI_Group worldGroup, newGroup;
	MPI_Comm comm1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_split(MPI_COMM_WORLD, rank % 3, rank, &comm1);

	if (rank % 3 == 0) {

		int n, newRank;
		MPI_Comm_rank(comm1, &newRank);
		MPI_Comm_size(comm1, &n);
		
		int* a = calloc(n * n, sizeof(int));
		int* loc_a = calloc(n, sizeof(int));

		if (a == NULL || loc_a == NULL) return -1;

		if (rank == MASTER) {
			for (int i = 0; i < n * n; i++)
				a[i] = i;

			printf("\nMatrica A:\n");
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					printf("%d\t", a[i * n + j]);
				}
				printf("\n");
			}
			printf("\n");
		}

		MPI_Scatter(a, n, MPI_INT, loc_a, n, MPI_INT, MASTER, comm1);

		int sum = 0;
		for (int i = 0; i < n; i++)
			sum += loc_a[i];

		if (sum < V)
			printf("Stari rank [%d]\tnovi rank [%d]\tsuma [%d]\n", rank, newRank, sum);

		free(a);
		free(loc_a);
	}

	MPI_Finalize();
	return 0;
}