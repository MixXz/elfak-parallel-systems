#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define N 1 << 12

//Napisati sekvencijalni program kojim se generiše skalarni proizvod dva vektora.Napisati
//OpenMP program kojim se generiše skalarni proizvod dva vektora, podelom iteracija petlje
//između različitih niti sa i bez korišćenja odredbe redukcije za kombinovanje parcijalnih rezultata
//u nitima.Uporediti vremena izvršenja u oba slučaja sa sekvencijalnim vremenom izvršenja.
//Uporediti ova rešenja za različite dimenzije vektora.

int main() {
	int a[N], b[N], cSeq, cPar, cParRedux;

	for (int i = 0; i < N; i++) {
		a[i] = rand() % 10;
		b[i] = rand() % 10;
	}

	cSeq = cPar = cParRedux = 0;
	double startTime = omp_get_wtime();
	for (int i = 0; i < N; i++) {
		cSeq += a[i] * b[i];
	}

	printf("Vreme potrebno za sekvencijalno izvrsenje: %f\n", omp_get_wtime() - startTime);

	startTime = omp_get_wtime();
	#pragma omp parallel for reduction(+ : cParRedux)
	for(int i = 0; i < N; i++) {
		cParRedux += a[i] * b[i];
	}

	printf("Vreme potrebno za paralelno izvrsenje sa redukcijom: %f\n", omp_get_wtime() - startTime);

	startTime = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		#pragma omp critical
		cPar += a[i] * b[i];
	}

	printf("Vreme potrebno za paralelno izvrsenje: %f\n", omp_get_wtime() - startTime);

	printf((cSeq == cPar) && (cSeq == cParRedux) ? "Rezultati se podudaraju.\n" : "Greska! Rezultati se ne podudaraju.\n");
}