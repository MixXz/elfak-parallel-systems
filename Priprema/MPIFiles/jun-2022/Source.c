﻿#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>
#define N 4
#define FIRST_FILE_NAME "jun-2022-prva.dat"
#define SECOND_FILE_NAME "jun-2022-druga.dat"

int main(int argc, char** argv)
{
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_File file;
	char buf[N];
	for (int i = 0; i < N; i++)
		buf[i] = rank + '0';

	//1.
	MPI_Offset offset = rank * N * sizeof(char);
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_seek(file, offset, MPI_SEEK_SET);
	MPI_File_write(file, buf, N, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//2.	
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_shared(file, buf, N, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	printf("Proces[%d] procitao:\n", rank);
	for (int i = 0; i < N; i++)
		printf("%c", buf[i]);

	//3
	char buf2[2 * N];
	for (int i = 0; i < N; i++)
		buf2[i] = buf2[i + N] = buf[i];

	MPI_Datatype fileType;
	MPI_Type_vector(2 * N, 2, 2 * N, MPI_CHAR, &fileType);
	MPI_Type_commit(&fileType);

	MPI_Offset disp = 2 * rank * sizeof(char);
	MPI_File_open(MPI_COMM_WORLD, SECOND_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, disp, MPI_CHAR, fileType, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, buf2, 2 * N, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	return 0;
}