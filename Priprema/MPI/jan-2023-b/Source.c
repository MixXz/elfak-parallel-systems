#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<mpi.h>
#include<stddef.h>
#define MASTER 0
#define DEFAULT_TAG 0
#define STUDENTS_COUNT 2
#define STRING_SIZE 20

//Da ne smara za scanf return value
#pragma warning( disable : 6031)

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	struct student {
		int index;
		char firstName[STRING_SIZE];
		char lastName[STRING_SIZE];
		float avgGrade;
	} students[STUDENTS_COUNT] = { {0, "", "", 0}, {0, "", "", 0} };

	MPI_Datatype studentType;

	const int blockLengths[4] = {1, STRING_SIZE, STRING_SIZE, 1};

	MPI_Aint displacements[4] = { offsetof(struct student, index),
								  offsetof(struct student, firstName),
								  offsetof(struct student, lastName),
								  offsetof(struct student, avgGrade)
								};

	MPI_Datatype types[4] = { MPI_INT, MPI_CHAR, MPI_CHAR, MPI_FLOAT };

	MPI_Type_create_struct(4, blockLengths, displacements, types, &studentType);
	MPI_Type_commit(&studentType);

	if (rank == MASTER) {
		for (int i = 0; i < STUDENTS_COUNT; i++) {
			scanf("%d", &students[i].index);
			scanf("%s", &students[i].firstName);
			scanf("%s", &students[i].lastName);
			scanf("%f", &students[i].avgGrade);
		}
	}

	MPI_Bcast(&students, STUDENTS_COUNT, studentType, MASTER, MPI_COMM_WORLD);

	printf("\n[Process %d]\n", rank);
	for (int i = 0; i < STUDENTS_COUNT; i++) {
		printf("Indeks: %d\n", students[i].index);
		printf("Ime: %s\n", students[i].firstName);
		printf("Prezime: %s\n", students[i].lastName);
		printf("Prosek: %.2f\n\n", students[i].avgGrade);
	}

	MPI_Finalize();
	return 0;
}