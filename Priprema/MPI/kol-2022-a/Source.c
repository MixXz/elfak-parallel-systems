#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#define K 4
#define N 2
#define M 3
#define MASTER 0

void initMatrix(int* mat, int rows, int cols);
void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int a[K][N], b[N][M], c[K][M];
	const int l = K / p;

	int* locA = calloc(l * N, sizeof(int)),
		* locC = calloc(l * M, sizeof(int));

	if (locC == NULL || locA == NULL) return -1;

	if (rank == MASTER) {
		initMatrix(&a[0][0], K, N);
		initMatrix(&b[0][0], N, M);
	}

	MPI_Datatype tempType, resizedTempType;
	MPI_Type_vector(l, N, N * p, MPI_INT, &tempType);
	MPI_Type_create_resized(tempType, 0, N * sizeof(int), &resizedTempType);
	MPI_Type_commit(&resizedTempType);

	MPI_Scatter(&a[0][0], 1, resizedTempType, locA, l * N, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&b[0][0], N * M, MPI_INT, MASTER, MPI_COMM_WORLD);

	for (int i = 0; i < l; i++)
		for (int j = 0; j < M; j++)
			for (int q = 0; q < N; q++)
				locC[i * M + j] += locA[i * N + q] * b[q][j];

	MPI_Type_vector(l, M, M * p, MPI_INT, &tempType);
	MPI_Type_create_resized(tempType, 0, M * sizeof(int), &resizedTempType);
	MPI_Type_commit(&resizedTempType);

	MPI_Gather(locC, l * M, MPI_INT, &c[0][0], 1, resizedTempType, MASTER, MPI_COMM_WORLD);

	printf("\n[Process %d] \n", rank);
	printMatrix(locA, l, N, "locA:\n");
	printMatrix(&b[0][0], N, M, "locB:\n");
	printMatrix(locC, l, M, "locC:\n");

	if (rank == MASTER) {
		printMatrix(&a[0][0], K, N, "\nMatrica A:\n");
		printMatrix(&b[0][0], N, M, "\nMatrica B:\n");
		printMatrix(&c[0][0], K, M, "\nMatrica C:\n");
	}

	free(locA);
	free(locC);

	MPI_Finalize();
	return 0;
}

void initMatrix(int* mat, int rows, int cols) {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			mat[i * cols + j] = i + j;
}

void printMatrix(int* mat, int rows, int cols, const char* msg) {
	msg&& printf(msg);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			printf("%d\t", mat[i * cols + j]);
		}
		printf("\n");
	}
}