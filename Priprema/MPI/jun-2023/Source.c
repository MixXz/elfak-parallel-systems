#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<mpi.h>
#include<stddef.h>

#define MASTER 0
#define P 2

#define N 4
#define STRING_SIZE 100

#pragma warning( disable : 6031)

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	struct employee {
		int id;
		char firstName[STRING_SIZE];
		char lastName[STRING_SIZE];
		float avgPlata;
	} employees[N], employeesLoc[N / P];

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			scanf("%d", &employees[i].id);
			scanf("%s", &employees[i].firstName);
			scanf("%s", &employees[i].lastName);
			scanf("%f", &employees[i].avgPlata);
		}
	}

	const int blocklens[4] = { 1, STRING_SIZE, STRING_SIZE, 1 };
	MPI_Aint displacements[4] = {
		offsetof(struct employee, id),
		offsetof(struct employee, firstName),
		offsetof(struct employee, lastName),
		offsetof(struct employee, avgPlata)
	};
	MPI_Datatype types[4] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_FLOAT };

	MPI_Datatype structType, sendType;
	MPI_Type_create_struct(4, blocklens, displacements, types, &structType);
	MPI_Type_commit(&structType);

	MPI_Type_contiguous(N / P, structType, &sendType);
	MPI_Type_create_resized(sendType, 0, (N / P) * sizeof(struct employee), &sendType);
	MPI_Type_commit(&sendType);

	MPI_Scatter(&employees[0], 1, sendType, &employeesLoc[0], N / P, structType, MASTER, MPI_COMM_WORLD);

	int locMinId = -1, minId;
	float minPlata = INT32_MAX;
	for (int i = 0; i < N / P; i++) {
		if (employeesLoc[i].avgPlata < minPlata) {
			minPlata = employeesLoc[i].avgPlata;
			locMinId = employeesLoc[i].id;
		}
	}

	MPI_Reduce(&locMinId, &minId, 1, MPI_INT, MPI_MIN, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER) {
		for (int i = 0; i < N; i++) {
			if (employees[i].id == minId) {
				printf("Najmanje placeni ima id %d, i platu %.2f\n", minId, employees[i].avgPlata);
			}
		}
	}

	MPI_Finalize();
	return 1;
}