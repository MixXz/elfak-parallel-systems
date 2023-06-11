#include <mpi.h>
#include <stdio.h>
#define MASTER 0
#define RIGHT_TAG 0
#define LEFT_TAG 1
#define M 4
#define N 4
#define K 6
#define P 3

void main(int argc, char** argv) {
	int rank, size;
	int a[M][N], b[N][K], c[M][K];
	int loc_b[N][K / P], loc_c[M][K / P];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == MASTER) {		
		printf("Matica A: \n");
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = i + j;
				printf("\t%d", a[i][j]);
			}
			printf("\n");
		}

		printf("\nMatica B: \n");
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < K; j++) {
				b[i][j] = (i * N) + j;
				printf("\t%d", b[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	}

	MPI_Bcast(&a[0][0], M * N, MPI_INT, MASTER, MPI_COMM_WORLD);

	MPI_Datatype sendType, resizedSendType;
	MPI_Type_vector(N, K / P, K, MPI_INT, &sendType);
	MPI_Type_create_resized(sendType, 0, (K / P) * sizeof(MPI_INT), &resizedSendType);
	MPI_Type_commit(&resizedSendType);

	MPI_Scatter(&b[0][0], 1, resizedSendType, &loc_b[0][0], N * (K / P), MPI_INT, MASTER, MPI_COMM_WORLD);

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < K / P; j++) {
			loc_c[i][j] = 0;
			for (int c = 0; c < N; c++)
				loc_c[i][j] += a[i][c] * loc_b[c][j];
		}
	}

	MPI_Datatype recvType, resizedRecvType;
	MPI_Type_vector(M, K / P, K, MPI_INT, &recvType);
	MPI_Type_create_resized(recvType, 0, (K / P) * sizeof(MPI_INT), &resizedRecvType);
	MPI_Type_commit(&resizedRecvType);

	MPI_Gather(&loc_c[0][0], M * (K / P), MPI_INT, &c[0][0], 1, resizedRecvType, MASTER, MPI_COMM_WORLD);
		
	if (rank == MASTER) {
		printf("\nMatica C: \n");
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < K; j++)
				printf("\t%d", c[i][j]);
			printf("\n");
		}
	}

	MPI_Finalize();
}