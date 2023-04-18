#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#define N 8
#define MASTER 0
#define DEFAULT_TAG 0

//Startuje se za 16 procesa.

void printArray(int arr[], int size, const char* message);
void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, size;
	int a[N][N], b[N], c[N];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	const int q = (int)sqrt(size);
	const int k = N / q;

	MPI_Datatype submatrixType;
	MPI_Type_vector(k, k, N, MPI_INT, &submatrixType);
	MPI_Type_commit(&submatrixType);

	int* locA = calloc(k * k, sizeof(int));
	int* locB = calloc(k, sizeof(int));

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			b[i] = i;
			for (int j = 0; j < N; j++)
				a[i][j] = i + j;
		}

		int cnt = 0;
		for (int i = 0; i < k; i++)
			for (int j = 0; j < k; j++)
				locA[cnt++] = a[i][j];

		int recvRank = 0;
		for (int i = 0; i < q; i++)
			for (int j = 0; j < q; j++) {
				if (recvRank == 0) { recvRank++; continue; }
				MPI_Send(&a[i * k][j * k], 1, submatrixType, recvRank++, DEFAULT_TAG, MPI_COMM_WORLD);
			}
	}
	else
		MPI_Recv(locA, k * k, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	MPI_Comm rowsComm, colsComm;
	int rowsRank, colsRank;

	MPI_Comm_split(MPI_COMM_WORLD, rank / q, rank % q, &rowsComm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, rank / q, &colsComm);
	MPI_Comm_rank(rowsComm, &rowsRank);
	MPI_Comm_rank(colsComm, &colsRank);

	if (colsRank == MASTER)
		MPI_Scatter(b, k, MPI_INT, locB, k, MPI_INT, MASTER, rowsComm);
	MPI_Bcast(locB, k, MPI_INT, MASTER, colsComm);

	int* locRes = calloc(k, sizeof(int));
	if (locRes == NULL) return -1;

	int ind = 0;
	for (int i = 0; i < k; i++) {
		locRes[i] = 0;
		for (int j = 0; j < k; j++)
			locRes[i] += locA[ind++] * locB[j];
	}

	int* res = calloc(N, sizeof(int));

	MPI_Gather(locRes, k, MPI_INT, res, k, MPI_INT, MASTER, colsComm);
	if (colsRank == MASTER)
		MPI_Reduce(res, c, N, MPI_INT, MPI_SUM, MASTER, rowsComm);

	printf("\n[Process %d]\n", rank);
	printMatrix(locA, k, k, "locA:\n");
	printArray(locB, k, "locB: ");
	printArray(locRes, k, "locRes: ");

	if (rank == MASTER) {
		printMatrix(&a[0][0], N, N, "\nMatrica A:\n");
		printArray(b, N, "\nVektor B:\n");
		printArray(c, N, "\nRezultujuci vektor C:\n");
	}

	free(locA);
	free(locB);
	free(locRes);
	free(res);

	MPI_Finalize();
	return 0;
}

void printArray(int arr[], int size, const char* message) {
	message&& printf(message);

	printf("[");
	for (int i = 0; i < size; i++)
		printf(i < size - 1 ? "%d, " : "%d]\n", arr[i]);
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