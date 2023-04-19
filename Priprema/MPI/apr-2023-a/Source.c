#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#define N 8
#define MASTER 0
#define DEFAULT_TAG 0

//za N = 8 program se startuje za 16 procesa
//za N = 4 program se startuje za 4 procesa

void printMatrix(int* mat, int rows, int cols, const char* msg);
void printVector(int arr[], int len, const char* msg);

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int a[N][N], b[N], c[N];
	const int q = (int)sqrt(size); // q predstavlja red velicine matrice procesa
	const int l = N / q; // l predstavlja broj redova u podmatrici A kao i broj elemenata podvektora B
	const int k = q + 1; // k predstavlja broj kolona u podmatrici A

	int* locA = calloc(l * k, sizeof(int));
	int* locB = calloc(l, sizeof(int));
	int* tmp = calloc(l, sizeof(int));
	int* rowsTmp = calloc(l, sizeof(int));

	MPI_Datatype submatrixType;
	MPI_Type_vector(l, k, N, MPI_INT, &submatrixType);
	MPI_Type_commit(&submatrixType);

	if (rank == MASTER) {
		int cnt = 0;
		for (int i = 0; i < N; i++) {
			b[i] = i;
			for (int j = 0; j < N; j++)
				a[i][j] = cnt++;
		}

		printMatrix(&a[0][0], N, N, "\nMatrica A:\n");
		printVector(b, N, "Vektor B: ");

		int proc = 0;
		for (int i = 0; i < q; i++)
			for (int j = 0; j < q; j++) {
				if (proc == MASTER) {
					for (int i = 0; i < l; i++)
						for (int j = 0; j < k; j++)
							locA[i * k + j] = a[i][j];
					proc++;
				}
				else
					MPI_Send(&a[i * l][j], 1, submatrixType, proc++, DEFAULT_TAG, MPI_COMM_WORLD);
			}
	}
	else
		MPI_Recv(locA, l * k, MPI_INT, MASTER, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	MPI_Datatype subvectorType, resizedSubvectorType;
	MPI_Type_vector(l, 1, q, MPI_INT, &subvectorType);
	MPI_Type_create_resized(subvectorType, 0, 1 * sizeof(int), &resizedSubvectorType);
	MPI_Type_commit(&resizedSubvectorType);

	MPI_Comm rowsComm, colsComm;
	MPI_Comm_split(MPI_COMM_WORLD, rank / q, rank % q, &rowsComm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, rank / q, &colsComm);

	int rowsRank, colsRank;
	MPI_Comm_rank(rowsComm, &rowsRank);
	MPI_Comm_rank(colsComm, &colsRank);

	if (colsRank == MASTER)
		MPI_Scatter(&b, 1, resizedSubvectorType, locB, l, MPI_INT, MASTER, rowsComm);
	MPI_Bcast(locB, l, MPI_INT, MASTER, colsComm);

	struct {
		int val;
		int rank;
	} locMin = { INT32_MAX, rank }, min = { INT32_MAX, -1 };

	for (int i = 0; i < l; i++)
		for (int j = 0; j < k; j++)
			if (locMin.val > locA[i * k + j])
				locMin.val = locA[i * k + j];

	MPI_Reduce(&locMin, &min, 1, MPI_2INT, MPI_MINLOC, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(&min, 1, MPI_2INT, MASTER, MPI_COMM_WORLD);

	//Mnoze se samo elementi nulte i poslednje kolone iz locA sa elementima iz locB
	for (int i = 0; i < l; i++)
		for (int j = 0; j < l; j++) {
			tmp[i] += locA[i * k + j * (k - 1)] * locB[j]; //Nacrtaj si
		}

	//U nultoj koloni matrice procesa su svi medjurezultati
	MPI_Reduce(tmp, rowsTmp, l, MPI_INT, MPI_SUM, MASTER, rowsComm);
	//skupljamo u master proces nulte kolone koji je i globalni.
	if (rowsRank == MASTER)
		MPI_Gather(rowsTmp, l, MPI_INT, &c[0], l, MPI_INT, min.rank, colsComm);

	printf("\n[Process %d]-----------------------\n", rank);
	printMatrix(locA, l, k, "locA:\n");
	printVector(locB, l, "locB: ");

	if (rank == min.rank) {
		printf("Ja, proces %d, sadrzim najmanji element matrice A.\n", rank);
		printVector(c, N, "Rezultujuci vektor C: ");
	}

	free(locA);
	free(locB);
	free(tmp);
	free(rowsTmp);

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

void printVector(int arr[], int len, const char* msg) {
	msg&& printf(msg);

	printf("[");
	for (int i = 0; i < len; i++)
		printf(i < len - 1 ? "%d, " : "%d]", arr[i]);
	printf("\n");
}