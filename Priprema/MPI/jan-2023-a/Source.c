#include<stdio.h>
#include<mpi.h>
#define N 4
#define K 2
#define MASTER 0
#define DEFAULT_TAG 0

//Potvrdila je asistentkinja Marija da ima dosta grešaka u tekstu zadatka počevši od one formule za
//broj procesa sa kojima se pokreće program pa do zahteva da se pronađe maksimalni element.

//Ovo ovako ne moze da se pomnozi...

//Traženje max elementa nema svrhe raditi jer se finalna matrica dobija u master procesu, da bi svi procesi učestvovali
//u traženju maksimalnog elementa matrice potrebno je opet distribuirati blokove po procesima što nema veze s vezom.

//Sa N = 4 i K = 2 program se startuje za 2 procesa.
//Sa N = 4 i K = 1 program se startuje za 4 procesa.

void initMatrix(int* mat, int rows, int cols, int zero);
void printMatrix(int* mat, int rows, int cols, const char* message);

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int a[N][N], b[N][N], c[N][N];
	int locA[K][N], locB[N][K], locC[N][N], tmp[N][N];

	MPI_Datatype rowsType, colsType;
	MPI_Type_vector(K, N, N, MPI_INT, &rowsType);
	MPI_Type_vector(N, K, N, MPI_INT, &colsType);
	MPI_Type_commit(&rowsType);
	MPI_Type_commit(&colsType);

	if (rank == MASTER) {
		initMatrix(&a[0][0], N, N, 0);
		initMatrix(&b[0][0], N, N, 0);

		for (int i = 0; i < K; i++)
			for (int j = 0; j < N; j++)
				locA[i][j] = a[i][j];

		for (int i = 0; i < N; i++)
			for (int j = 0; j < K; j++)
				locB[i][j] = a[i][j];

		for (int i = 1; i < size; i++) {
			MPI_Send(&a[i * K][0], 1, rowsType, i, DEFAULT_TAG, MPI_COMM_WORLD);
			MPI_Send(&b[0][i * K], 1, colsType, i, DEFAULT_TAG, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(locA, K * N, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(locB, K * N, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	MPI_Reduce(&locC, &c, N * N, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	printf("\n[Process %d]\n", rank);
	printMatrix(&locA[0][0], K, N, "locA:\n");
	printMatrix(&locB[0][0], N, K, "locB:\n");

	if (rank == MASTER) {
		printMatrix(&a[0][0], N, N, "\nMatrica A:\n");
		printMatrix(&b[0][0], N, N, "\nMatrica B:\n");
		printMatrix(&c[0][0], N, N, "\nMatrica C:\n");
	}

	MPI_Finalize();
	return 0;
}

void initMatrix(int* mat, int rows, int cols, int zero) {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			mat[i * cols + j] = zero ? 0 : i + j;
}

void printMatrix(int* mat, int rows, int cols, const char* message) {
	message&& printf(message);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			printf("%d\t", mat[i * cols + j]);
		printf("\n");
	}
}
