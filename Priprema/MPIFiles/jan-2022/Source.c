#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define NUM_OF_PROCESSES 4
#define FILE_SIZE 1024

#define PROCESS_CHUNK_SIZE FILE_SIZE / NUM_OF_PROCESSES
#define N_CHARS PROCESS_CHUNK_SIZE / sizeof(char)

#define FIRST_FILENAME "jan-2022-file1.dat"
#define SECOND_FILENAME "jan-2022-file2.dat"

int main(int argc, char** argv)
{
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char buf[N_CHARS];
	MPI_File file;

	//Ovo na ispitu ne.
	//---------------------------------------------------------------------------------------
	//Inicijalizacija fajla od 1 KB. (zamišljamo da je 10MB)
	//Sa char sam radio da može da se čita fajl... na ispitu samo sve u unsigned long long...
	for (int i = 0; i < N_CHARS; i++) {
		buf[i] = rank + '0';
	}

	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_write_at(file, rank * PROCESS_CHUNK_SIZE, buf, N_CHARS, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);
	//---------------------------------------------------------------------------------------

	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_read_shared(file, buf, N_CHARS, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	printf("Process[%d] procitao: ", rank);
	for (int i = 0; i < N_CHARS; i++)
		printf("%c", buf[i]);

	MPI_Datatype fileType;
	const int partSize = N_CHARS / 2;
	MPI_Type_vector(2, partSize, size * partSize, MPI_CHAR, &fileType);
	MPI_Type_commit(&fileType);

	MPI_File_open(MPI_COMM_WORLD, SECOND_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, rank * partSize, MPI_CHAR, fileType, "native", MPI_INFO_NULL);
	MPI_File_write_all(file, buf, N_CHARS, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	MPI_Finalize();
	return 0;
}