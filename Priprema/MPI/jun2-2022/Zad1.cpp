#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

#define MASTER 0

#define M 4
#define N 4

int main(int argc, char** argv) {
	int rank, p;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (N % p != 0) {
		MPI_Finalize();
		exit(-1);
	}

	int a[M][N], b[N], c[N];

	if (rank == MASTER) {
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = rand() % 5 + 1;
			}
		}

		for (int i = 0; i < N; i++) {
			b[i] = rand() % 3 + 1;
		}
	}

	const int l = N / p;
	int* loc_a = (int*)malloc(l * M * sizeof(int));
	int* loc_b = (int*)malloc(l * sizeof(int));
	MPI_Datatype send_type;

	MPI_Type_vector(l * M, 1, p, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	MPI_Scatter(&a, 1, send_type, loc_a, l * M, MPI_INT, MASTER, MPI_COMM_WORLD);

	MPI_Type_vector(l, 1, p, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	MPI_Scatter(&b, 1, send_type, loc_b, l, MPI_INT, MASTER, MPI_COMM_WORLD);

	int* loc_c = (int*)calloc(N, sizeof(int));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < l; j++) {
			loc_c[i] += loc_a[i * l + j] * loc_b[j];
		}
	}

	MPI_Reduce(loc_c, &c, N, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			printf("%d ", c[i]);
		}
	}

	MPI_Finalize();
	free(loc_a);

	return 0;
}