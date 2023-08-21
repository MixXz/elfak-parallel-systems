#include<iostream>
#include<omp.h>

using namespace std;

#define N 256
#define THREADS_NUM 8

int main() {
	int d_seq, d_par, a_seq[N], a_par[N];
	int b[N], c[N], z[N], pom = 0;

	for (int i = 0; i < N; i++) {
		z[i] = rand() % 5;
		b[i] = rand() % 4;
		c[i] = rand() % 3;
	}

	//Sekvencijalno******************************************
	d_seq = 2;
	for (int i = 1; i < N; i++) {
		d_seq *= z[i];
		a_seq[i] = b[i] * c[pom + N - i - 1] + a_seq[i - 1];
	}
	//*******************************************************

	//Paralelno**********************************************
	d_par = 2;
	#pragma omp parallel for reduction(* : d_par) 
	for (int i = 1; i < N; i++) {
		d_par *= z[i];
		a_par[i] = b[i] * c[pom + N - i - 1];
	}

	//Ovaj deo mora sekvencijalno zbog loop-carry zavisnosti.
	for(int i = 1; i < N; i++) {
		a_par[i] += a_par[i - 1];
	}
	//*******************************************************

	for (int i = 1; i < N; i++) {
		if (a_seq[i] != a_par[i] || d_seq != d_par) {
			cout << "Not ok." << endl;
			return 0;
		}
	}

	cout << "Ok." << endl;

	return 0;
}