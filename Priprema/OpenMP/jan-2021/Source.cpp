#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define THREADS_NUM 8

int main() {
	int aSeq[N], aPar[N], aTemp[N],
		b[N], c[N], tSeq, tPar;

	tSeq = tPar = 1;
	for (int i = 0; i < N; i++) {
		aSeq[i] = aPar[i] = b[i] = c[i] = i;
	}

	for (int i = 0; i < N - 1; i++) {
		aSeq[i] = aSeq[i + 1] + b[i] * c[i];
		tSeq *= aSeq[i];
	}

	omp_set_num_threads(THREADS_NUM);
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < N - 1; i++) {
			aTemp[i] = aPar[i + 1];
		}

		#pragma omp for reduction(* : tPar)
		for (int i = 0; i < N - 1; i++) {
			aPar[i] = aTemp[i] + b[i] * c[i];
			tPar *= aPar[i];
		}
	}

	if (tSeq != tPar) {
		cout << "Not ok." << endl;
		return 0;
	}

	cout << "Ok." << endl;

	return 0;
}