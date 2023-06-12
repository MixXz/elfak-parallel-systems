#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define N 15
#define FIRST_FILE_NAME "jun-2021-file1.dat"
#define SECOND_FILE_NAME "jun-2021-file2.dat"

int main(int argc, char** argv)
{
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int writeBuf[N], readBuff[N];

	MPI_File file;
	MPI_Offset offset = (size - rank - 1) * sizeof(int) * N;

	for (int i = 0; i < N; i++)
		writeBuf[i] = rank;

	//1
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_write_at(file, offset, writeBuf, N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//2
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_at(file, offset, readBuff, N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	int ok = 1;
	for (int i = 0; i < N; i++) {
		if (readBuff[i] != writeBuf[i]) {
			ok = 0;
			break;
		}
	}

	printf("Process[%d]: ", rank);
	printf(ok ? "OK.\n" : "Not OK!\n");

	//3
	int val = N;
	int count = 0;
	while (val > 0) {
		val -= ++count;
	}

	int* blockLenghts = (int*)malloc(count * sizeof(int));
	int* displacements = (int*)malloc(count * sizeof(int));

	blockLenghts[0] = 1;
	displacements[0] = 0;

	for (int i = 1; i < count; i++) {
		blockLenghts[i] = i + 1;
		displacements[i] = displacements[i - 1] + i * size + rank;
	}

	MPI_Datatype fileType;
	MPI_Type_indexed(count, blockLenghts, displacements, MPI_INT, &fileType);
	MPI_Type_commit(&fileType);
	free(blockLenghts);
	free(displacements);

	MPI_File_open(MPI_COMM_WORLD, SECOND_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, rank * sizeof(int), MPI_INT, fileType, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, readBuff, N, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	return 0;
}