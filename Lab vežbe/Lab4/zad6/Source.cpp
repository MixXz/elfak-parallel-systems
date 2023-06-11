#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<omp.h>

using namespace std;

#define M 1 << 8
#define N 1 << 8

//Napisati OpenMP kod koji sadrži sledeću petlju:
//for (i = 0; i < m; i++)
//	for (j = 0; j < n; j++)
//		a[i][j] = 2 * a[i - 1][j];
//i izvršiti njenu paralelizaciju.Proučiti da li postoje zavisnosti između iteracija i po kom indeksu
//je moguća paralelizacija.Ispitati da li zamena petlji uzrokuje promenu u performansama.

int main() {
	int aSeq[M][N], aPar[M][N];

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			aSeq[i][j] = aPar[i][j] = rand() % 10;
		}
	}

	double startTime = omp_get_wtime();
	for (int i = 1; i < M; i++)
		for (int j = 0; j < N; j++)
			aSeq[i][j] = 2 * aSeq[i - 1][j];

	cout << "Vreme potrebno za sekvencijalno izvrsenje: " << omp_get_wtime() - startTime << endl;

	startTime = omp_get_wtime();
	#pragma omp parallel for 
	for (int j = 0; j < N; j++)
		for (int i = 1; i < M; i++)
			aPar[i][j] = 2 * aPar[i - 1][j];


	cout << "Vreme potrebno za paralelno izvrsenje: " << omp_get_wtime() - startTime << endl;

	int ok = 1;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			if (aSeq[i][j] != aPar[i][j]) {
				ok = 0;
				break;
			}
		}
	}

	printf(ok ? "Rezultati se podudaraju.\n" : "Greska! Rezultati se ne podudaraju.\n");

	return 0;
}