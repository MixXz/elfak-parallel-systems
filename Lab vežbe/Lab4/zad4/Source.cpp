#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<omp.h>

using namespace std;

#define N 256000

//Napisati OpenMP program kojim se generiše prosečna vrednost elemanata vektora, podelom
//iteracija petlje između različitih niti sa i bez korišćenja odredbe redukcije za kombinovanje
//parcijalnih rezultata u nitima.Uporediti vremena izvršenja u oba slučaja sa sekvencijalnim
//vremenom izvršenja.Uporediti ova rešenja za različite vrednosti dimenizija vektora.

int main() {
	int* vec = (int*)calloc(N, sizeof(int));
	int sum = 0;
	double startTime, avgSeq, avgPar, avgParRedux;

	if (vec == NULL) return 0;

	for (int i = 0; i < N; i++)
		vec[i] = rand() % 10;

	startTime = omp_get_wtime();
	for (int i = 0; i < N; i++)
		sum += vec[i];

	avgSeq = (double)sum / N;

	cout << "Vreme potrebno za sekvencijalno izvrsenje: " << omp_get_wtime() - startTime << endl;
	
	sum = 0;
	startTime = omp_get_wtime();
	#pragma omp parallel for reduction(+: sum)
	for (int i = 0; i < N; i++)
		sum += vec[i];

	avgParRedux = (double)sum / N;

	cout << "Vreme potrebno za paralelno izvrsenje sa redukcijom: " << omp_get_wtime() - startTime << endl;

	sum = 0;
	startTime = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < N; i++)
		#pragma omp critical
		sum += vec[i];

	avgPar = (double)sum / N;

	cout << "Vreme potrebno za paralelno izvrsenje: " << omp_get_wtime() - startTime << endl;

	printf((avgSeq == avgPar) && (avgSeq == avgParRedux) ? "Rezultati se podudaraju.\n" : "Greska! Rezultati se ne podudaraju.\n");

	return 0;
}