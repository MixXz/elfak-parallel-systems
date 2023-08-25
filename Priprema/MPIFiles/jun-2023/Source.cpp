#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define K 6
#define FILENAME "fajl.dat"

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int buff[K];
	for (auto i = 0; i < K; i++) {
		buff[i] = i + rank * K;
	}

	MPI_File file;

	//a)
	MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_seek(file, rank * K * sizeof(int), MPI_SEEK_SET);
	MPI_File_write(file, &buff, K, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//b) - pravimo se da ne znamo koliko je podataka upisano jer tako tražila na ispitu kad sam polagao.
	MPI_Offset file_size;

	MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_get_size(file, &file_size);

	const MPI_Offset proc_chunk = file_size / size;
	const int nints = proc_chunk / sizeof(int);

	int* read_buff = (int*)malloc(proc_chunk);
	MPI_File_read_at(file, rank * K * sizeof(int), read_buff, nints, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	for (auto i = 0; i < nints; i++) {
		if (read_buff[i] != buff[i]) {
			printf("Greska! Citanje nije pravilno izrseno!");
			return 0;
		}
	}

	//c)
	MPI_Datatype file_type;
	MPI_Type_vector(size, nints / size, nints, MPI_INT, &file_type);
	MPI_Type_commit(&file_type);

	MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, rank * (nints / size) * sizeof(int), MPI_INT, file_type, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, read_buff, nints, MPI_INT, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	free(read_buff);

	return 0;
}