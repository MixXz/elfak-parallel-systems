#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define M 6

#define FIRST_FILENAME "jun2-2022-file1.dat"
#define SECOND_FILENAME "jun2-2022-file2.dat"

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//1
	MPI_File file;
	char buf[M];
	for (int i = 0; i < M; i++)
		buf[i] = rank + '0';

	MPI_Offset offset = rank * M * sizeof(char);
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_seek(file, offset, MPI_SEEK_SET);
	MPI_File_write_all(file, buf, M, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//2
	char readBuff[M];
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_at(file, offset, readBuff, M, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	int isOk = 1;
	for (int i = 0; i < M; i++) {
		if (buf[i] != readBuff[i]) {
			isOk = 0;
			break;
		}
	}

	printf("Process[%d] proverio i kaze: ", rank);
	printf(isOk ? "Ok.\n" : "Not ok.\n");

	//3
	MPI_Datatype fileType;
	MPI_Type_vector(size, M / size, M, MPI_CHAR, &fileType);
	MPI_Type_commit(&fileType);

	MPI_File_open(MPI_COMM_WORLD, SECOND_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, rank * (M / size) * sizeof(char), MPI_CHAR, fileType, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, readBuff, M, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	return 0;
}
