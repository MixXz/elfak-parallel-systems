#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define M 120

int main() {
	int x_seq, x_par, b_seq[N][M], b_par[N][M];
	int a[N][M];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			a[i][j] = rand() % 5;
		}
	}

	x_seq = 0;
	for (int i = 2; i < N; i++) {
		for (int j = 0; j < M; j++) {
			x_seq += a[i][j];
			b_seq[i][j] = 4 * b_seq[i - 2][j];
		}
	}

	x_par = 0;
	#pragma omp parallel for reduction(+ : x_par)
	for (int j = 0; j < M; j++) {
		for (int i = 2; i < N; i++) {
			x_par += a[i][j];
			b_par[i][j] = 4 * b_par[i - 2][j];
		}
	}

	if (x_seq != x_par) {
		cout << "Not ok." << endl;
		return 0;
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (b_seq[i][j] != b_par[i][j]) {
				cout << "Not ok." << endl;
				return 0;
			}
		}
	}

	cout << "Ok." << endl;

	return 0;
}