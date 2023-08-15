#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#define MASTER 0
#define FILENAME "rez.dat"

#define K 8
#define N 4

int main(int argc, char** argv) {
	int rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (K % p != 0) {
		printf("K mora biti deljivo sa brojem procesa.\n");
		MPI_Finalize();
		exit(-1);
	}

	int a[K][N], b[N];

	if (rank == MASTER) {
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = rand() % 5 + 1;
			}
		}

		for (int i = 0; i < N; i++) {
			b[i] = rand() % 5 + 1;
		}
	}

	const int l = K / p;
	int* loc_a = (int*)malloc(l * N * sizeof(int));

	MPI_Datatype send_type;
	MPI_Type_vector(l, N, p * N, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, N * sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	MPI_Scatter(&a, 1, send_type, loc_a, l * N, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&b, N, MPI_INT, MASTER, MPI_COMM_WORLD);

	int* loc_c = (int*)calloc(l, sizeof(int));

	for (int i = 0; i < l; i++) {
		for (int j = 0; j < N; j++) {
			loc_c[i] += loc_a[i * N + j] * b[j];
		}
	}

	MPI_Datatype file_type;
	MPI_Type_vector(l, 1, p, MPI_INT, &file_type);
	MPI_Type_commit(&file_type);

	MPI_File file;
	MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, rank * sizeof(int), MPI_INT, file_type, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, loc_c, l, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	free(loc_a);
	free(loc_c);

	return 0;
}