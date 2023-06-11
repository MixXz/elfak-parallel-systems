#include<mpi.h>
#include<stdio.h>
#define MASTER 0
#define N 4
#define P 4

void printMatrix(int mat[N][N], const char* message) {
	printf(message);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			printf("\t%d", mat[i][j]);
		printf("\n");
	}
	printf("\n");
}

void printArray(int arr[], int rank) {
	printf("Proces %d: [", rank);
	for (int i = 0; i < N; i++)
		printf(i < N - 1 ? "%d, " : "%d]\n", arr[i]);
}

int findMax(int arr[], int len) {
	int max = 0;
	for (int i = 0; i < len; i++)
		max = arr[i] > max ? arr[i] : max;

	return max;
}

void main(int argc, char** argv) {
	int rank, size;
	int a[N][N], b[N][N], c[N][N];
	int loc_a[N], loc_c[N];

	struct {
		int val;
		int rank;
	} loc_max, max;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == MASTER) {
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++) {
				a[i][j] = (i * N) + j;
				b[i][j] = i + j;
			}
		printMatrix(a, "Matrica A:\n");
		printMatrix(b, "Matrica B:\n");
	}

	MPI_Scatter(&a[0][0], N, MPI_INT, &loc_a[0], N, MPI_INT, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&b[0][0], N * N, MPI_INT, MASTER, MPI_COMM_WORLD);

	for(int i = 0; i < N; i++) {
		loc_c[i] = 0;
		for (int j = 0; j < N; j++)
			loc_c[i] += loc_a[j] * b[j][i];
	}
	
	MPI_Gather(&loc_c[0], N, MPI_INT, &c[0][0], N, MPI_INT, MASTER, MPI_COMM_WORLD);



	MPI_Datatype sendType, resizedSendType;
	MPI_Type_vector(N, 1, N, MPI_INT, &sendType);
	MPI_Type_create_resized(sendType, 0, sizeof(MPI_INT), &resizedSendType);
	MPI_Type_commit(&resizedSendType);

	MPI_Scatter(&c[0][0], 1, resizedSendType, &loc_c[0], N, MPI_INT, MASTER, MPI_COMM_WORLD);

	loc_max = { findMax(loc_c, N), rank };
	printf("Proces %d, lokalni maksimum matrice C: %d\n", rank, loc_max.val);

	MPI_Reduce(&loc_max, &max, 1, MPI_2INT, MPI_MAXLOC, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER) {
		printMatrix(c, "Matrica C:\n");
		printf("Globalni maksimum matrice C je %d, i nalazi se u procesu %d.\n", max.val, max.rank);
	}

	MPI_Finalize();
}
