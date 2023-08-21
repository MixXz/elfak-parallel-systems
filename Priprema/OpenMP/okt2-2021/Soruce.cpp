#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define M 120

int main() {
	int h_seq, h_par, a_seq[N][M], a_par[N][M];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			a_seq[i][j] = a_par[i][j] = rand() % 5;
		}
	}

	h_seq = 0;
	for (int i = 2; i < N; i++) {
		for (int j = 0; j < M; j++) {
			h_seq += a_seq[i][j];
			a_seq[i][j] = a_seq[i - 2][j];
		}
	}

	h_par = 0;
	#pragma omp parallel for reduction(+ : h_par)
	for (int j = 0; j < M; j++) {
		for (int i = 2; i < N; i++) {
			h_par += a_par[i][j];
			a_par[i][j] = a_par[i - 2][j];
		}
	}

	if (h_seq != h_par) {
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