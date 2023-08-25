#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define FILENAME_1 "fajl.dat"
#define FILENAME_2 "fajl1.dat"
#define FILENAME_3 "fajl2.dat"

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_File file;
	MPI_Offset file_size;
	MPI_File_open(MPI_COMM_WORLD, FILENAME_1, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_get_size(file, &file_size);

	const MPI_Offset proc_chunk = file_size / size;
	const int nints = proc_chunk / sizeof(int);

	int* read_buff = (int*)malloc(proc_chunk);
	MPI_File_read_ordered(file, read_buff, nints, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//a)
	MPI_File_open(MPI_COMM_WORLD, FILENAME_2, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_write_shared(file, read_buff, nints, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//b) - nisam proverio!
	const int nints_tenth = nints / 10;

	MPI_Datatype file_type;
	MPI_Type_vector(10, nints_tenth, nints_tenth * size, MPI_INT, &file_type);
	MPI_Type_commit(&file_type);
	
	MPI_File_open(MPI_COMM_WORLD, FILENAME_3, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, (size - rank - 1) * nints_tenth * sizeof(int), MPI_INT, file_type, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, read_buff, nints_tenth, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	free(read_buff);
	return 0;
}