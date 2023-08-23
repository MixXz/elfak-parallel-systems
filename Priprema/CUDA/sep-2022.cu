%%cu
#include<iostream>
#include<stdlib.h>

using namespace std;

#define N 100

/*
Da li je validno rešenje? -jeste.
Da li je najoptimalnije? -nije.
*/

__host__ void init(float* a, float * vec, int c);
__device__ float operatorX(int a, int b, float c);
__global__ void kernel(float* dev_a, float* out_vec, int* c);

int main(void) {
    float a[N][N];
    float vec[N];
    int c = 5;

    init(&a[0][0], &vec[0], c);

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            cout << a[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    for(int i = 0; i < N; i++) {
        cout << vec[i] << " ";
    }
    cout << endl;

    return 0;
}

__host__ void init(float* a, float * vec, int c) {
    float *dev_a, *dev_vec;
    int *dev_c;

    size_t a_size = N * N * sizeof(float);
    size_t vec_size = N * sizeof(float);

    cudaMalloc(&dev_a, a_size);
    cudaMalloc(&dev_vec, vec_size);
    cudaMalloc(&dev_c, sizeof(int));

    cudaMemcpy(dev_a, a, a_size, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_c, &c, sizeof(int), cudaMemcpyHostToDevice);

    kernel<<<N, 256>>>(dev_a, dev_vec, dev_c);

    cudaMemcpy(a, dev_a, a_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(vec, dev_vec, vec_size, cudaMemcpyDeviceToHost);

    cudaFree(dev_a);
    cudaFree(dev_vec);
    cudaFree(dev_c);
}

__device__ float operatorX(int a, int b, float c)
{
    return a * c - b;
}

__global__ void kernel(float* dev_a, float* out_vec, int* c) {
    int i = blockIdx.x;
    int j = threadIdx.x;

    if(j >= N) return;

    __shared__ float row_sum;

    float el = operatorX(i, j, (float) *c);
    dev_a[i * N + j] = el;

    atomicAdd(&row_sum, el);

    if(j == N - 1){
        out_vec[i] = row_sum;
    }
}