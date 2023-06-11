#include <mpi.h>
#include <stdio.h>
#define MASTER 0
#define RIGHT_TAG 0
#define LEFT_TAG 1
#define N 8
#define P 4

void printMatrix(int mat[N][N], int rank) {
	printf("Proces %d: \n", rank);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			printf("\t%d ", mat[i][j]);
		printf("\n");
	}
	printf("\n");
}

void main(int argc, char** argv) {

	int rank, size;
	int mat[N][N];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == MASTER) {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				mat[i][j] = i * N + j;
		printMatrix(mat, rank);

		for (int i = 1; i < P; i++) {
			MPI_Datatype sendType, resizedSendType;
			MPI_Type_vector(N - i, 1, N + 1, MPI_INT, &sendType);
			MPI_Type_create_resized(sendType, 0, sizeof(MPI_INT), &resizedSendType);
			MPI_Type_commit(&resizedSendType);

			MPI_Send(&mat[0][i], 1, resizedSendType, i, RIGHT_TAG, MPI_COMM_WORLD);
			MPI_Send(&mat[i][0], 1, resizedSendType, i, LEFT_TAG, MPI_COMM_WORLD);
		}

	}
	else {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				mat[i][j] = 0;

		MPI_Recv(&mat[0][0], N - rank, MPI_INT, MASTER, RIGHT_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&mat[1][0], N - rank, MPI_INT, MASTER, LEFT_TAG, MPI_COMM_WORLD, &status);
		printMatrix(mat, rank);
	}

	MPI_Finalize();
}