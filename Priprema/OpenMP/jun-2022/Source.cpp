#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define THREADS_NUM 8

int main() {

	int xSeq[N], xPar[N],
		ySeq[N], yPar[N],
		z[N], gSeq, gPar;

	gSeq = gPar = 0;
	for (int i = 0; i < N; i++) {
		xSeq[i] = xPar[i] = ySeq[i] = yPar[i] = z[i] = rand() % 5;
	}

	for (int i = 1; i < N; i++) {
		ySeq[i] += xSeq[i - 1];
		xSeq[i] += z[i];
		gSeq += z[i - 1];
	}

	omp_set_num_threads(THREADS_NUM);
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 1; i < N; i++) {
			xPar[i] += z[i];
		}

		#pragma omp for reduction(+ : gPar)
		for (int i = 1; i < N; i++) {
			yPar[i] += xPar[i - 1];
			gPar += z[i - 1];
		}
	}
	for (int i = 0; i < N; i++) {
		if (xSeq[i] != xPar[i]
			|| ySeq[i] != yPar[i]
			|| gSeq != gPar) {
			cout << "Not ok." << endl;
			return 0;
		}
	}

	cout << "Ok." << endl;

	return 0;
}