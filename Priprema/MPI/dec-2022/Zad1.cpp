#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

#define MASTER 0
#define N 4

void printMatrix(int* mat, int rows, int cols, const char* msg);

int main(int argc, char** argv) {
	int rank, p;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int a[N][N], b[N][N], c[N][N];

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = rand() % 4 + 1;
				b[i][j] = rand() % 4 + 1;
			}
		}
	}

	const int q = (int)sqrt(p);

	MPI_Comm row_comm, col_comm;
	MPI_Comm_split(MPI_COMM_WORLD, rank / q, rank % q, &row_comm);
	MPI_Comm_split(MPI_COMM_WORLD, rank % q, rank / q, &col_comm);

	int row_rank, col_rank;
	MPI_Comm_rank(row_comm, &row_rank);
	MPI_Comm_rank(col_comm, &col_rank);

	//Slanje blokova matrice a **********************************************************
	const int l = N / q;	
	int* loc_a = (int*)malloc(l * N * sizeof(int));

	MPI_Datatype send_type;
	MPI_Type_vector(l, N, q * N, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, N * sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	if (row_rank == MASTER) {
		MPI_Scatter(&a[0][0], 1, send_type, loc_a, l * N, MPI_INT, MASTER, col_comm);
	}		
	MPI_Bcast(loc_a, l * N, MPI_INT, MASTER, row_comm);
	//***********************************************************************************

	//Slanje blokova matrice b **********************************************************
	int* loc_b = (int*)malloc(l * N * sizeof(int));

	MPI_Type_vector(l * N, 1, q, MPI_INT, &send_type);
	MPI_Type_create_resized(send_type, 0, sizeof(int), &send_type);
	MPI_Type_commit(&send_type);

	if (col_rank == MASTER) {
		MPI_Scatter(&b[0][0], 1, send_type, loc_b, l * N, MPI_INT, MASTER, row_comm);
	}
	MPI_Bcast(loc_b, l * N, MPI_INT, MASTER, col_comm);
	//************************************************************************************

	//Množenje

	MPI_Finalize();
	free(loc_a);
	free(loc_b);
	return 0;
}