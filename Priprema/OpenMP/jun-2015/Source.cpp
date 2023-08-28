#include<iostream>
#include<omp.h>

using namespace std;

#define N 10

int main() {
	int aseq[N], apar[N], snseq, snpar, p;
	
	snseq = 1;
	p = 5;
	for (int i = 0; i < N; i++) {
		aseq[i] = snseq;
		snseq *= p;
	}

	snpar = 1;
	#pragma omp parallel for 
	for (int i = 0; i < N; i++) {
		apar[i] = snpar;
		snpar *= p;
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