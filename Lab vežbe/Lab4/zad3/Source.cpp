#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<omp.h>

using namespace std;

#define N 1 << 16

//Napisati OpenMP program kojim se pronalazi ukupan broj prostih brojeva između 1 i N,
//podelom iteracija petlje između različitih niti sa i bez korišćenja odredbe redukcije za
//kombinovanje parcijalnih rezultata u nitima.Uporediti vremena izvršenja u oba slučaja sa
//sekvencijalnim vremenom izvršenja.Uporediti ova rešenja za različite vrednosti N.

bool isPrime(int num);

int main() {
	int cntSeq, cntPar, cntParRedux;
	cntSeq = cntPar = cntParRedux = 0;

	double startTime = omp_get_wtime();
	for (int i = 0; i < N; i++)
		isPrime(i) && cntSeq++;

	cout << "Vreme potrebno za sekvencijalno izvrsenje: " << omp_get_wtime() - startTime << endl;

	startTime = omp_get_wtime();
	#pragma omp parallel for reduction(+ : cntParRedux)
	for (int i = 0; i < N; i++)
		isPrime(i) && cntParRedux++;

	cout << "Vreme potrebno za paralelno izvrsenje sa redukcijom: " << omp_get_wtime() - startTime << endl;

	startTime = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		#pragma omp critical
		isPrime(i) && cntPar++;
	}

	cout << "Vreme potrebno za paralelno izvrsenje: " << omp_get_wtime() - startTime << endl;

	printf((cntSeq == cntPar) && (cntSeq == cntParRedux) ? "Rezultati se podudaraju.\n" : "Greska! Rezultati se ne podudaraju.\n");

}

bool isPrime(int num)
{
    if (num <= 1)
        return false;

	for (int i = 2; i < num; i++)
		if (num % i == 0)
			return false;

	return true;
}