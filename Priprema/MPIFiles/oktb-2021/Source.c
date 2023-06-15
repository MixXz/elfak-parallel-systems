#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define FIRST_FILENAME "okt-2021-file1.dat"
#define SECOND_FILENAME "okt-2021-file2.dat"
#define THIRD_FILENAME "okt-2021-file3.dat"

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_File file;

	/*Inicijalizacija fajla... ne treba na ispitu...---------------------------------------------------*/
	char buf[5];
	for (int i = 0; i < 5; i++)
		buf[i] = rank + '0';

	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_seek(file, rank * 5 * sizeof(char), MPI_SEEK_SET);
	MPI_File_write_all(file, buf, 5, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);
	/*------------------------------------------------------------------------------------------------*/

	//Čitanje - pravimo se da ne znamo koliki je fajl.
	MPI_Offset fileSize;
	MPI_File_open(MPI_COMM_WORLD, FIRST_FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	MPI_File_get_size(file, &fileSize);

	int myChunk = fileSize / size;
	int nChars = myChunk / sizeof(char);
	char* buffer = (char*)malloc(nChars * sizeof(char));

	MPI_Offset readOffset = rank * nChars * sizeof(char);
	MPI_File_read_at_all(file, readOffset, buffer, nChars, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	printf("Process[%d] procitao: ", rank);
	for (int i = 0; i < nChars; i++)
		printf("%c", buffer[i]);

	//1
	MPI_File_open(MPI_COMM_WORLD, SECOND_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_write_shared(file, buffer, nChars, MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&file);

	//2
	MPI_Datatype fileType;
	int oneFifthOfNChars = nChars / 5;
	MPI_Type_vector(5, oneFifthOfNChars, size * oneFifthOfNChars, MPI_CHAR, &fileType);
	MPI_Type_commit(&fileType);

	MPI_Offset viewOffset = rank * oneFifthOfNChars * sizeof(char);
	MPI_File_open(MPI_COMM_WORLD, THIRD_FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
	MPI_File_set_view(file, viewOffset, MPI_CHAR, fileType, "native", MPI_INFO_NULL);

	MPI_File_write_all_begin(file, buffer, nChars, MPI_CHAR);
	//Nesto radimo između kao
	MPI_File_write_all_end(file, buffer, MPI_STATUS_IGNORE);

	free(buffer);
	MPI_Finalize();
	return 0;
}