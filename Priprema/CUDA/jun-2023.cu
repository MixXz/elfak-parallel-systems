%%cu
#include<iostream>
using namespace std;

#define N 256
#define SIZE 32

__host__ void init(int *a, int *min);
__global__ void extractDiag(int *dev_a, int *out_diag);
__global__ void findMin(int *vec, int *out_min);

int main(void) {
    int a[N][N], min;

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            a[i][j] = rand() % 20;
        }
    }

    init(&a[0][0], &min);

    cout << "Min: " << min << endl;

    return 0;
}

__host__ void init(int *a, int *min) {
    int *dev_a, *out_diag, *out_min;
    size_t size_a = N * N * sizeof(int);
    size_t size_diag = N * sizeof(int);

    cudaMalloc(&dev_a, size_a);
    cudaMalloc(&out_diag, size_diag);
    cudaMalloc(&out_min, sizeof(int));

    cudaMemcpy(dev_a, a, size_a, cudaMemcpyHostToDevice);

    dim3 grid(ceil(N / SIZE));
    dim3 block(SIZE);
    extractDiag<<<grid, block>>>(dev_a, out_diag);

    dim3 grid2(ceil(N / SIZE / 2));
    findMin<<<grid2, block>>>(out_diag, out_min);

    cudaMemcpy(min, out_min, sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(dev_a);
    cudaFree(out_diag);
    cudaFree(out_min);
}

__global__ void extractDiag(int *dev_a, int *out_diag) {
    int tid = threadIdx.x + blockIdx.x * blockDim.x;

    if(tid >= N) {
        return;
    }

    out_diag[tid] = dev_a[tid * N + tid];
}

__global__ void findMin(int *vec, int *out_min) {
    int tid = threadIdx.x + blockIdx.x * blockDim.x * 2; 
 
    if(tid >= N) {
        return;
    }

    __shared__ int partials[SIZE];
    partials[threadIdx.x] = vec[tid + blockDim.x];

    if(vec[tid] < partials[threadIdx.x]) {
        partials[threadIdx.x] = vec[tid];
    }

    for(int stride = blockDim.x / 2; stride > threadIdx.x; stride /= 2) {
        if(partials[threadIdx.x] > partials[threadIdx.x + stride]) {
            partials[threadIdx.x] = partials[threadIdx.x + stride];
        }        
        __syncthreads();
    }

    if(threadIdx.x == 0) {
      *out_min = partials[0];
    }
}