%%cu
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

using namespace std;

#define N 1024
#define P 0.5
#define BLOCK_SIZE 256

__host__ void initProc(int *a, int *b, float *c);
__global__ void kernelProc(int *devA, int *devB, float *outC);

int main(void) {
    int a[N], b[N];
    float c[N - 2];

    for(int i = 0; i < N; i++)
        a[i] = b[i] = i;

    initProc(a, b, c);

    cout << "C: " << endl;
    for(int i = 0; i < N - 2; i++)
        cout << c[i] << " ";

    return 0;
}

__host__ void initProc(int *a, int *b, float *c) {
    int *devA, *devB;
    float *outC;
    size_t sizeAB = N * sizeof(int);
    size_t sizeC = (N - 2) * sizeof(int);

    cudaMalloc(&devA, sizeAB);
    cudaMalloc(&devB, sizeAB);
    cudaMalloc(&outC, sizeC);

    cudaMemcpy(devA, a, sizeAB, cudaMemcpyHostToDevice);
    cudaMemcpy(devB, b, sizeAB, cudaMemcpyHostToDevice);

    kernelProc<<<BLOCK_SIZE, BLOCK_SIZE>>>(devA, devB, outC);

    cudaMemcpy(c, outC, sizeC, cudaMemcpyDeviceToHost);

    cudaFree(devA);
    cudaFree(devB);
    cudaFree(outC);
}

__global__ void kernelProc(int *devA, int *devB, float *outC) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if(tid > N) return;

    __shared__ int shA[BLOCK_SIZE];
    __shared__ int shB[BLOCK_SIZE];

    shA[threadIdx.x] = devA[tid];
    shB[threadIdx.x] = devB[tid];

    __syncthreads();

    if(tid >= N - 2) return;

    if(threadIdx.x < blockDim.x - 2)
        outC[tid] = (shA[threadIdx.x] + shA[threadIdx.x + 1] + shA[threadIdx.x + 2]) * P + (shB[threadIdx.x] + shB[threadIdx.x + 1] + shB[threadIdx.x + 2]) * (1 - P);
    else if (threadIdx.x < blockDim.x - 1)
        outC[tid] = (shA[threadIdx.x] + shA[threadIdx.x + 1] + devA[tid + 2]) * P + (shB[threadIdx.x] + shB[threadIdx.x + 1] + devB[tid + 2]) * (1 - P);
    else
        outC[tid] = (shA[threadIdx.x] + devA[tid + 1] + devA[tid + 2]) * P + (shB[threadIdx.x] + devB[tid + 1] + devB[tid + 2]) * (1 - P);
}