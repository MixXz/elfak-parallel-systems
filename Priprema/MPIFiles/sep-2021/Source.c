#include<stdio.h>
#include<mpi.h>
#include<math.h>
#define M 6
#define N 9
#define DATA_COUNT 9

#define FIRST_FILE_NAME "sep-2021-file1.dat"
#define SECOND_FILE_NAME "sep-2021-file2.dat"

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int writeBuf[DATA_COUNT], readBuf[DATA_COUNT];

	for (int i = 0; i < DATA_COUNT; i++)
		writeBuf[i] = rank;

	MPI_File file;
	MPI_Offset offset = (size - rank - 1) * DATA_COUNT * sizeof(int);

	//1
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_seek(file, offset, MPI_SEEK_SET);
	MPI_File_write_all(file, writeBuf, DATA_COUNT, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//2
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILE_NAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_at(file, offset, readBuf, DATA_COUNT, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	int isOk = 1;
	for (int i = 0; i < DATA_COUNT; i++) {
		if (writeBuf[i] != readBuf[i]) {
			isOk = 0;
			break;
		}
	}

	printf("Process[%d]: ", rank);
	printf(isOk ? "Ok.\n" : "Not Ok.\n");

	//3
	int submatrixSize = (int)sqrt(DATA_COUNT);
	int gsizes[] = { M, N },
		distribs[] = { MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK },
		dargs[] = { MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG },
		psizes[] = { M / submatrixSize, N / submatrixSize };

	MPI_Datatype fileType;
	MPI_Type_create_darray(size, rank, 2, gsizes, distribs, dargs, psizes, MPI_ORDER_C, MPI_INT, &fileType);
	MPI_Type_commit(&fileType);

	MPI_File_open(MPI_COMM_WORLD, SECOND_FILE_NAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, 0, MPI_INT, fileType, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, readBuf, DATA_COUNT, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	return 0;
}