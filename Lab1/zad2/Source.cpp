#include <mpi.h>
#include <stdio.h>
#include <math.h>
#define MASTER 0
#define N 8
#define P 4
#define TMP_LEN (N * N) / (2 * P)

//Napisati MPI program koji pronalazi maksimalnu vrednost u delu matrice reda n(n-parno) 
//koga cine vrste matrice sa parnim indeksom(i = 0, 2, 4, ...).Matrica je inicijalizovana
//u master procesu(P0).Svaki proces treba da dobije elemente vrsta sa parnim indeksom iz
//odgovarajucih n / p kolona(p - broj procesa, n deljivo sa p) i nadje lokalni maksimum.Na
//taj nacin, P0 dobija elemente vrsta sa parnim indeksom iz prvih n / p kolona i nalazi
//lokalni maksimum, P1 dobija elemente kolona sa parnim indeksom iz sledecih n / p kolona
//i nalazi lokalni maksimum itd.Nakon toga, u master procesu se izracunava i na ekranu
//prikazuje globalni maksimum u trazenom delu matrice.Zadatak realizovati koriscenjem
//iskljucivo grupnih operacija i izvedenih tipova podataka

void printMatrix(int mat[N][N]) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			printf("\t%d ", mat[i][j]);
		printf("\n");
	}
}

void printArray(int arr[], int len, int rank) {
	printf("Proces %d: [", rank);
	for (int i = 0; i < len; i++)
		printf(i < len - 1 ? "%d, " : "%d]\n", arr[i]);
}

int findMax(int arr[], int len) {
	int max = 0;
	for (int i = 0; i < len; i++)
		max = arr[i] > max ? arr[i] : max;

	return max;
}

int main(int argc, char** argv) {

	int rank, size;
	int mat[N][N], tmp[TMP_LEN];

	struct {
		int value;
		int rank;
	} loc_max, max;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == MASTER) {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				mat[i][j] = (i + 1) * (j + 1);

		printMatrix(mat);
	}

	MPI_Datatype sendType, resizedSendType;
	MPI_Type_vector(N / 2, N / P, 2 * N, MPI_INT, &sendType);
	MPI_Type_create_resized(sendType, 0, (N / P) * sizeof(MPI_INT), &resizedSendType);
	MPI_Type_commit(&resizedSendType);

	MPI_Scatter(&mat[0][0], 1, resizedSendType, &tmp[0], TMP_LEN, MPI_INT, MASTER, MPI_COMM_WORLD);
	printArray(tmp, TMP_LEN, rank);

	loc_max = { findMax(tmp, TMP_LEN), rank };
	MPI_Reduce(&loc_max, &max, 1, MPI_2INT, MPI_MAXLOC, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER)
		printf("Globalni maksimum je %d, i nalazi se u procesu %d.", max.value, max.rank);

	MPI_Finalize();
}