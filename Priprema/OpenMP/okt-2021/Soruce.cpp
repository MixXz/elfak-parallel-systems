#include<iostream>
#include<omp.h>

using namespace std;

#define N 50
#define M 50
#define THREADS_NUM 8

int main() {

	int bSeq[N][M], bPar[N][M],
		a[N][M], xSeq, xPar;

	xSeq = xPar = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			a[i][j] = rand() % 5;
		}
	}

	for (int i = 2; i < N; i++) {
		for (int j = 0; j < M; j++) {
			xSeq += a[i][j];
			bSeq[i][j] = a[i - 2][j];
		}
	}

	#pragma omp parallel for reduction(+ : xPar)
	for (int i = 2; i < N; i++) {
		for (int j = 0; j < M; j++) {
			xPar += a[i][j];
			bPar[i][j] = a[i - 2][j];
		}
	}

	for (int i = 2; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (bSeq[i][j] != bPar[i][j] || xSeq != xPar) {
				cout << "Not ok." << endl;
				return 0;
			}
		}
	}

	cout << "Ok." << endl;

	return 0;
}