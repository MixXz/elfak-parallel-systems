#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<omp.h>

using namespace std;

#define N 1 << 22

//Napisati OpenMP program kojim se generiše maksimalna vrednost elemanata vektora,
//podelom iteracija petlje između različitih niti korišćenjem direktive critical za kombinovanje
//parcijalnih rezultata u nitima.Uporediti vremena izvršenja u oba slučaja sa sekvencijalnim
//vremenom izvršenja.Uporediti ova rešenja za različite vrednosti dimenizija vektora.

int main() {
	int* vec = (int*)calloc(N, sizeof(int));
	int maxSeq, maxPar;

	if (vec == NULL) return 0;

	maxSeq = maxPar = INT32_MIN;
	for (int i = 0; i < N; i++)
		vec[i] = rand() % 10;

	double startTime = omp_get_wtime();
	for (int i = 0; i < N; i++)
		if(vec[i] > maxSeq)
			maxSeq = vec[i];

	cout << "Vreme potrebno za sekvencijalno izvrsenje: " << omp_get_wtime() - startTime << endl;

	startTime = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < N; i++)
		if (vec[i] > maxPar)
			#pragma omp critical
			maxPar = vec[i];

	cout << "Vreme potrebno za paralelno izvrsenje: " << omp_get_wtime() - startTime << endl;

	printf(maxSeq == maxPar ? "Rezultati se podudaraju.\n" : "Greska! Rezultati se ne podudaraju.\n");

	return 0;
}