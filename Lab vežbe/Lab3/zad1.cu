%%cu
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

#define N 4
#define BLOCK_SIZE 32

__host__ void initProc(int *a, int *b, int len);
__global__ void kernelProc(int *a, int *b, int len);

int main(void) {
    int a[N + 2], b[N];

    cout << "A ";
    for(int i = 0; i < N + 2; i++) {
        a[i] = rand() % 100;
        cout << a[i] << " ";
    }

    initProc(a, b, N);

    cout << "\nResult: ";
    for(int i = 0; i < N; i++)
        cout << b[i] << " ";

    return 0;
}

__host__ void initProc(int *a, int *b, int len) {
    int* devA, *devB;

    size_t sizeA = (len + 2) * sizeof(int);
    size_t sizeB = len * sizeof(int);

    cudaMalloc(&devA, sizeA);
    cudaMalloc(&devB, sizeB);

    cudaMemcpy(devA, a, sizeA, cudaMemcpyHostToDevice);

    kernelProc<<<BLOCK_SIZE, BLOCK_SIZE>>>(devA, devB, len);

    cudaMemcpy(b, devB, sizeB, cudaMemcpyDeviceToHost);

    cudaFree(devA);
    cudaFree(devB);
}

__global__ void kernelProc(int *a, int *b, int len) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if(tid > len) return;

    __shared__ int sh[BLOCK_SIZE];
    sh[threadIdx.x] = a[tid + 1];

    __syncthreads();

    int pom;
    if(threadIdx.x == 0)
       pom = 3 * a[tid] + 10 * sh[threadIdx.x] + 7 * sh[threadIdx.x + 1];
    else if(threadIdx.x == blockDim.x - 1)
        pom = 3 * sh[threadIdx.x - 1] + 10 * sh[threadIdx.x] + 7 * a[tid + 2];
    else
        pom = 3 * sh[threadIdx.x - 1] + 10 * sh[threadIdx.x] + 7 * sh[threadIdx.x + 1];

    b[tid] = pom / 20.f;
}