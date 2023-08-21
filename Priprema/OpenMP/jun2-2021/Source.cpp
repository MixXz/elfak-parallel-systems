#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define M 120

int main() {
	int x_seq, x_par, a_seq[N][M], a_par[N][M];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			a_seq[i][j] = a_par[i][j] = rand() % 5;
		}
	}

	x_seq = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			x_seq += a_seq[i][j];
			a_seq[i][j] = 2 * a_seq[i][j];
		}
	}

	x_par = 0;
	#pragma omp parallel for reduction(+ : x_par)
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			x_par += a_par[i][j];
			a_par[i][j] = 2 * a_par[i][j];
		}
	}

	if (x_seq != x_par) {
		cout << "Not ok." << endl;
		return 0;
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (a_seq[i][j] != a_par[i][j]) {
				cout << "Not ok." << endl;
				return 0;
			}
		}
	}

	cout << "Ok." << endl;

	return 0;
}