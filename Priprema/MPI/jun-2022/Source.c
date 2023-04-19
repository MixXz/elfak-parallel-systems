#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#define N 4
#define MASTER 0
#define DEFAULT_TAG 0

void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int a[N][N], b[N][N], c[N][N];
	const int q = (int)sqrt(size);
	const int k = N / q;

	int* locA = calloc(k * k, sizeof(int)),
		* locB = calloc(k * k, sizeof(int)),
		* locC = calloc(k * k, sizeof(int)),
		* locARows = calloc(k * N, sizeof(int)),
		* locBCols = calloc(k * N, sizeof(int));

	if (locA == NULL || locB == NULL || locC == NULL) return -1;

	//Slanje blokova (podmatrica) matrice A i B svim procesima --------------------------------
	MPI_Datatype submatrixType;
	MPI_Type_vector(k, k, N, MPI_INT, &submatrixType);
	MPI_Type_commit(&submatrixType);

	if (rank == MASTER) {
		int cnt = 0;
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++) {
				a[i][j] = cnt++;
				b[i][j] = i + j;
			}

		for (int i = 0; i < k; i++)
			for (int j = 0; j < k; j++) {
				locA[i * k + j] = a[i][j];
				locB[i * k + j] = b[i][j];
			}

		int proc = 0;
		for (int i = 0; i < q; i++)
			for (int j = 0; j < q; j++) {
				if (proc == MASTER) { proc++; continue; }
				MPI_Send(&a[i * k][j * k], 1, submatrixType, proc, DEFAULT_TAG, MPI_COMM_WORLD);
				MPI_Send(&b[i * k][j * k], 1, submatrixType, proc++, DEFAULT_TAG, MPI_COMM_WORLD);
			}
	}
	else {
		MPI_Recv(locA, k * k, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(locB, k * k, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	//Podela komunikatora ------------------------------------------------------------------------
	MPI_Comm rowsComm, colsComm;
	MPI_Comm_split(MPI_COMM_WORLD, rank / q, rank % q, &rowsComm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, rank / q, &colsComm);

	int rowsRank, colsRank;
	MPI_Comm_rank(rowsComm, &rowsRank);
	MPI_Comm_rank(colsComm, &colsRank);

	//Distribucija blokova po redovima ----------------------------------------------------------
	MPI_Datatype gatherType, resizedGatherType;
	MPI_Type_vector(k, k, N, MPI_INT, &gatherType);
	MPI_Type_create_resized(gatherType, 0, k * sizeof(int), &resizedGatherType);
	MPI_Type_commit(&resizedGatherType);

	MPI_Gather(locA, k * k, MPI_INT, locARows, 1, resizedGatherType, MASTER, rowsComm);
	MPI_Bcast(locARows, k * N, MPI_INT, MASTER, rowsComm);

	//Distribucija blokova po kolonama ----------------------------------------------------------
	MPI_Gather(locB, k * k, MPI_INT, locBCols, k * k, MPI_INT, MASTER, colsComm);
	MPI_Bcast(locBCols, N * k, MPI_INT, MASTER, colsComm);

	//Množenje ----------------------------------------------------------------------------------
	for (int i = 0; i < k; i++)
		for (int j = 0; j < k; j++)
			for (int p = 0; p < N; p++)
				locC[i * k + j] += locARows[i * N + p] * locBCols[p * k + j];

	//Prikupljanje rezultata---------------------------------------------------------------------
	//Ovo stvarno ne znam da li moze na neki nacin sa gather
	if (rank != MASTER)
		MPI_Send(locC, k * k, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD);

	if (rank == MASTER) {
		int proc = 0;
		for (int i = 0; i < q; i++)
			for (int j = 0; j < q; j++) {
				if (proc == 0) {
					for (int p = 0; p < k; p++)
						for (int z = 0; z < k; z++)
							c[p][z] = locC[p * k + z];
					proc++;
				}
				else
					MPI_Recv(&c[i * k][j * k], 1, resizedGatherType, proc++, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			}
	}

	//Stampa-------------------------------------------------------------------------------------
	printf("\n[Process %d]-----------------<\n", rank);
	printMatrix(locARows, k, N, "locARows:\n");
	printMatrix(locBCols, N, k, "locBCols:\n");
	printMatrix(locC, k, k, "locC:\n");

	if (rank == MASTER) {
		printMatrix(&a[0][0], N, N, "\nMatrica A:\n");
		printMatrix(&b[0][0], N, N, "\nMatrica B:\n");
		printMatrix(&c[0][0], N, N, "\nMatrica C:\n");
	}

	free(locA);
	free(locB);
	free(locARows);
	free(locBCols);

	MPI_Finalize();
	return 0;
}

void printMatrix(int* mat, int rows, int cols, const char* msg) {
	msg&& printf(msg);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			printf("%d\t", mat[i * cols + j]);
		printf("\n");
	}
}