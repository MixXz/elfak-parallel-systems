#include<iostream>
#include<omp.h>

using namespace std;

#define N 10

int main() {
	int bseq[N], bpar[N], dseq, dpar;
	int init = 5; // ako d nije 0 inicijalno.

	for (int i = 0; i < N; i++) {
		bseq[i] = bpar[i] = rand() % 6;
	}

	dseq = init;
	for (int m = 0; m < N; m++) {
		bseq[m] = bseq[m + 1] * dseq;
		dseq += m;
	}

	int bpom[N];
	for (int i = 0; i < N; i++) {
		bpom[i] = bpar[i + 1];
	}

	dpar = init;
	#pragma omp parallel for reduction(+ : dpar)
	for (int m = 0; m < N; m++) {
		bpar[m] = bpom[m] * (m * (m - 1) / 2 + init);
		dpar += m;
	}

	if (dpar != dseq) {
		cout << "not ok" << endl;
		return 0;
	}

	for (int i = 0; i < N; i++) {
		if (bseq[i] != bpar[i]) {
			cout << "not ok" << endl;
			return 0;
		}
	}

	cout << "ok" << endl;
	return 0;
}