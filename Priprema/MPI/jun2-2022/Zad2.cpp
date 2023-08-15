#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#include<stddef.h>

#define MASTER 0
#define LEN 50
#define NUM_OF_STUDENTS 2

#define is_diag_rank(rank, n) (rank % (n + 1) == 0)

struct student {
	int index;
	char first_name[LEN];
	char last_name[LEN];
	float avg;
};

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	struct student students[NUM_OF_STUDENTS];

	if (rank == MASTER) {
		students[0] = { 17782, "Petar", "Petrovic", 6 };
		students[1] = { 17783, "Mika", "Mikic", 9 };
	}

	const int n = (int)sqrt(size);

	MPI_Comm diag_comm;
	MPI_Comm_split(MPI_COMM_WORLD, is_diag_rank(rank, n), 0, &diag_comm);

	MPI_Datatype student_type;
	const int blocklengths[4] = {
		1,
		LEN,
		LEN,
		1
	};
	const MPI_Aint displacements[4] = {
		offsetof(struct student, index),
		offsetof(struct student, first_name),
		offsetof(struct student, last_name),
		offsetof(struct student, avg),
	};
	const MPI_Datatype types[4] = {
		MPI_INT,
		MPI_CHAR,
		MPI_CHAR,
		MPI_FLOAT
	};

	MPI_Type_create_struct(4, blocklengths, displacements, types, &student_type);
	MPI_Type_commit(&student_type);

	if (is_diag_rank(rank, n)) {
		MPI_Bcast(&students, NUM_OF_STUDENTS, student_type, MASTER, diag_comm);
	}

	MPI_Finalize();
	return 1;
}