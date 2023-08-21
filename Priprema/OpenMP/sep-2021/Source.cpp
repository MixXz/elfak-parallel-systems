#include<iostream>
#include<omp.h>

using namespace std;

#define N 50000

int main() {
	int j, x_seq, x_par;
	int b_seq[2 * N + 2], b_par[2 * N + 2];
	int a[N];

	for (int i = 0; i < N; i++) {
		a[i] = rand() % 4;
		b_seq[i] = b_par[i] = rand() % 5;
	}

	j = N + 1;
	x_seq = 0;
	for (int i = 0; i < N; i++) {
		x_seq += a[i];
		b_seq[i] += b_seq[j++];
	}

	j = N + 1;
	x_par = 0;
	#pragma omp parallel for reduction(+ : x_par) firstprivate(j)
	for (int i = 0; i < N; i++) {
		x_par += a[i];
		b_par[i] += b_par[j + i];
	}

	if (x_seq != x_par) {
		cout << "Not ok." << endl;
		return 0;
	}

	for (int i = 0; i < 2 * N + 2; i++) {
		if (b_seq[i] != b_par[i]) {
			cout << "Not ok." << endl;
			return 0;
		}
	}

	cout << "Ok." << endl;

	return 0;
}