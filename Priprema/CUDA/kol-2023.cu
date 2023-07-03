%%cu
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

#define N 8
#define BLOCK_SIZE 32

using namespace std;

__host__ void initProc(int *A, int *B);
__global__ void kernelProc(int *devA, int *outB);

int main(void) {
    int A[N + 2], B[N];

    for(int i = 0; i < N + 2; i++)
        A[i] = 5;
    
    initProc(A, B);

    for(int i = 0; i < N; i++)
        cout << B[i] << " ";

    return 0;
}

__host__ void initProc(int *A, int *B) {
    int* devA, *outB;

    cudaMalloc(&devA, (N + 2) * sizeof(int));
    cudaMalloc(&outB, N * sizeof(int));

    cudaMemcpy(devA, A, (N + 2) * sizeof(int), cudaMemcpyHostToDevice);

    kernelProc<<<BLOCK_SIZE, BLOCK_SIZE>>>(devA, outB);

    cudaMemcpy(B, outB, N * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(devA);
    cudaFree(outB);
}

__global__ void kernelProc(int *devA, int *outB) {
    int tid = threadIdx.x + blockDim.x * blockIdx.x;

    if(tid > N) return;

    __shared__ int shA[BLOCK_SIZE];
    shA[threadIdx.x] = devA[tid];

    __syncthreads();

    if(threadIdx.x < blockDim.x - 2)
        outB[tid] = 0.3 * shA[threadIdx.x] + 0.4 * shA[threadIdx.x + 1] + 0.3 * shA[threadIdx.x + 2];
    else if(threadIdx.x < blockDim.x - 1)
        outB[tid] = 0.3 * shA[threadIdx.x] + 0.4 * shA[threadIdx.x + 1] + 0.3 * devA[tid + 2];
    else
        outB[tid] = 0.3 * shA[threadIdx.x] + 0.4 * devA[tid + 1] + 0.3 * devA[tid + 2];
}