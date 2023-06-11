#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define N 500

//Napisati sekvencijalni program kojim se vrši množenje dve matrice A i B reda N.
//Korišćenjem OpenMP direktive izvršiti paralelizaciju petlje, tako da se izvrši distribucija
//iteracija između niti.
//Podesiti broj niti tako da bude jednak broju jezgara računara.Izmeriti vreme množenja matrica
//za sekvencijalni slučaj i za slučaj posle uvođenja openMP direktiva.Uporediti oba rešenja za
//različite dimenzije matrica.

int main() {
	int a[N][N], b[N][N], cSec[N][N], cPar[N][N];

	omp_set_num_threads(omp_get_num_procs());

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			a[i][j] = rand() % 10;
			b[i][j] = rand() % 10;
			cSec[i][j] = cPar[i][j] = 0;
		}
	}

	double start = omp_get_wtime();

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				cSec[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	printf("Vreme za sekvencijalno izvrsenje: %f\n", omp_get_wtime() - start);

	start = omp_get_wtime();

	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				cPar[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	printf("Vreme za paralelno izvrsenje: %f\n", omp_get_wtime() - start);

	return 0;
}