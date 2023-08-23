%%cu
#include<iostream>

using namespace std;

#define N 32
#define SIZE 8;

__host__ void init(int *a, int *b, int *c);
__global__ void kernel(int *dev_a, int *dev_b, int *out_c);

int main(void) {
    int a[N], b[N], c[N];

    for(int i = 0; i < N; i++) {
        a[i] = rand() % 5;
        b[i] = rand() % 3;
    }

    init(&a[0], &b[0], &c[0]);

    for(int i = 0 ; i < N; i++) {
        cout << c[i] << " ";
    }
    cout << endl;

    return 0;
}

__host__ void init(int *a, int *b, int *c) {
    int *dev_a, *dev_b, *out_c;
    size_t size = N * sizeof(int);

    cudaMalloc(&dev_a, size);
    cudaMalloc(&dev_b, size);
    cudaMalloc(&out_c, size);

    cudaMemcpy(dev_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, size, cudaMemcpyHostToDevice);

    kernel<<<N / SIZE / 2, SIZE>>>(dev_a, dev_b, out_c);

    cudaMemcpy(c, out_c, size, cudaMemcpyDeviceToHost);

    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(out_c);
}

__global__ void kernel(int *dev_a, int *dev_b, int *out_c) {
    __shared__ int psum[SIZE];
    int i = threadIdx.x + blockIdx.x * (blockDim.x * 2);

    psum[threadIdx.x] = dev_a[i] * dev_b[i + blockDim.x];
    __syncthreads();

    for(int s = blockDim.x / 2; s > 0; s /= 2) {
        if(threadIdx.x < s) {
            psum[threadIdx.x] += psum[threadIdx.x + s];
        }
        __syncthreads();
    }

    if(threadIdx.x == 0) {
        out_c[blockIdx.x] = psum[0];
    }
}