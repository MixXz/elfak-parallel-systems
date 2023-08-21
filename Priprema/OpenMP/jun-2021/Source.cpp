#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define THREADS_NUM 8

int main() {
	int sumSeq[N], sumPar[N], sumTemp[N],
		res[N], add[N], xSeq, xPar;

	for (int i = 0; i < N; i++) {
		sumSeq[i] = sumPar[i] = res[i] = add[i] = rand() % 5;
	}

	for (int i = N - 1; i > 1; i--)
	{
		xSeq = res[i] + add[i];
		sumSeq[i] = sumSeq[i - 1] + xSeq;
	}

	omp_set_num_threads(THREADS_NUM);
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = N - 1; i > 1; i--) {
			sumTemp[i] = sumPar[i - 1];
		}

		#pragma omp for lastprivate(xPar)
		for (int i = N - 1; i > 1; i--) {
			xPar = res[i] + add[i];
			sumPar[i] = sumTemp[i] + xPar;
		}
	}

	for (int i = 0; i < N; i++) {
		if (sumSeq[i] != sumPar[i]) {
			cout << "Not Ok." << endl;
			return 0;
		}
	}

	cout << "Ok." << endl;

	return 0;
}