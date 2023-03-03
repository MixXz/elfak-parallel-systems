#include <mpi.h>
#include <stdio.h>
#define MASTER 0
#define N 8
#define P 4
#define TMP_LEN (N * N) / (2 * P)

//Napisati MPI program koji pronalazi minimalnu vrednost u delu matrice reda n(n - parno)
//koga cine kolone matrice sa parnim indeksom(j = 0, 2, 4, ...).Matrica je inicijalizovana u
//master procesu(P0).Svaki proces treba da dobije elemente kolona sa parnim indeksom iz
//odgovarajucih n / p vrsta(p - broj procesa, n deljivo sa p) i nadje lokalni minimum.Na taj
//nacin, P0 dobija elemente kolona sa parnim indeksom iz prvih n / p vrsta i nalazi lokalni
//minimum, P1 dobija elemente kolona sa parnim indeksom iz sledecih n / p vrsta i nalazi
//lokalni minimum itd.Nakon toga, u master procesu se izracunava i na ekranu prikazuje
//globalni minimum u trazenom delu matrice.Zadatak realizovati koriscenjem iskljucivo
//grupnih operacija i izvedenih tipova podataka.

int findMin(int arr[], int len) {
    int min = arr[0];
    for (int i = 0; i < len; i++)
        min = arr[i] < min ? arr[i] : min;

    return min;
}

void printArray(int arr[], int len, int rank) {
    printf("Proces %d: [", rank);
    for (int i = 0; i < len; i++)
        printf(i < len - 1 ? "%d, " : "%d]\n", arr[i]);
}

void printMatrix(int mat[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("\t%d ", mat[i][j]);
        printf("\n");
    }
}

int main(int argc, char** argv) {

    int rank, size;
    int mat[N][N], tmp[TMP_LEN];

    struct {
        int value;
        int rank;
    } loc_min, min;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == MASTER) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                mat[i][j] = i + j;

        printMatrix(mat);
    }

    MPI_Datatype sendType, resizedSendType;
    MPI_Type_vector(N / 2, 1, 2, MPI_INT, &sendType);
    MPI_Type_create_resized(sendType, 0, N * sizeof(MPI_INT), &resizedSendType);
    MPI_Type_commit(&resizedSendType);

    MPI_Scatter(&mat[0][0], N / P, resizedSendType, &tmp[0], TMP_LEN, MPI_INT, MASTER, MPI_COMM_WORLD);
    printArray(tmp, TMP_LEN, rank);

    loc_min = { findMin(tmp, TMP_LEN), rank };
    MPI_Reduce(&loc_min, &min, 1, MPI_2INT, MPI_MINLOC, MASTER, MPI_COMM_WORLD);

    if (rank == MASTER)
        printf("Globalni minimum je %d i nalazi se u procesu %d.\n", min.value, min.rank);

    MPI_Finalize();
}