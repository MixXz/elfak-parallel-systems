#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define MASTER 0
#define P 2

#define K 2
#define M 4
#define L 6

void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int a[K][M], b[M][L], c[K][L];
	int locA[K][M / P], locB[M / P][L], locC[K][L];

	if (rank == MASTER) {
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < M; j++) {
				a[i][j] = rand() % 5 + 1;
				for (int q = 0; q < L; q++) {
					b[j][q] = rand() % 5 + 1;
				}
			}
		}
		printMatrix(&a[0][0], K, M, "Matrica A:\n");
		printMatrix(&b[0][0], M, L, "Matrica B:\n");
	}

	int blocklens[K * (M / P)],
		displacements[K * (M / P)];

	for (int i = 0; i < K * (M / P); i++) {
		displacements[i] = i * M / P;
		blocklens[i] = 1;
	}

	MPI_Datatype sendType;
	MPI_Type_indexed(K * (M / P), blocklens, displacements, MPI_INT, &sendType);
	MPI_Type_create_resized(sendType, 0, sizeof(int), &sendType);
	MPI_Type_commit(&sendType);

	MPI_Scatter(&a[0][0], 1, sendType, &locA[0][0], K * (M / P), MPI_INT, MASTER, MPI_COMM_WORLD);

	MPI_Type_vector(M / P, L, L * (M / P), MPI_INT, &sendType);
	MPI_Type_create_resized(sendType, 0, L * sizeof(int), &sendType);
	MPI_Type_commit(&sendType);

	MPI_Scatter(&b[0][0], 1, sendType, &locB[0][0], L * (M / P), MPI_INT, MASTER, MPI_COMM_WORLD);

	struct {
		int val;
		int rank;
	} locMin = { INT32_MAX, rank }, min = { INT32_MAX, -1 };

	for (int i = 0; i < K; i++) {
		for (int j = 0; j < M / P; j++) {
			if (locA[i][j] < locMin.val) {
				locMin.val = locA[i][j];
				locMin.rank = rank;
			}
		}
	}

	MPI_Reduce(&locMin, &min, 1, MPI_2INT, MPI_MINLOC, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&min, 1, MPI_2INT, MASTER, MPI_COMM_WORLD);

	for (int i = 0; i < K; i++) {
		for (int j = 0; j < L; j++) {
			locC[i][j] = c[i][j] = 0;

			for (int q = 0; q < M / P; q++) {
				locC[i][j] += locA[i][q] * locB[q][j];
			}
		}
	}

	MPI_Reduce(&locC[0][0], &c[0][0], K * L, MPI_INT, MPI_SUM, min.rank, MPI_COMM_WORLD);

	if (rank == min.rank) {
		printf("Processs[%d]:\n", rank);
		printMatrix(&c[0][0], K, L, "C:\n");
	}

	MPI_Finalize();
	return 1;
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

