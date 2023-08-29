#include<iostream>
#include<omp.h>

using namespace std;

#define N 10

int main() {
	int aseq[N], apar[N], mseq, mpar;
	int init = 5; // ako m nije 0 inicijalno.

	mseq = init;
	for (int i = 0; i < N; i++) {
		aseq[i] = mseq;
		mseq += i;
	}
	mpar = init;
	#pragma omp parallel for reduction(+ : mpar)
	for (int i = 0; i < N; i++) {
		apar[i] = i * (i - 1) / 2 + init;
		mpar += i;
	}

	if (mpar != mseq) {
		cout << "not ok" << endl;
		return 0;
	}

	for (int i = 0; i < N; i++) {
		if (aseq[i] != apar[i]) {
			cout << "not ok" << endl;
			return 0;
		}
	}

	cout << "ok" << endl;
	return 0;
}