/*
Author: Zhaodong Yang
Class: ECE6122
Last Date Modified: 2021/11/22
Description:
This file is to use cuda to calculate the interior temperature of a thin plate using Jocabi update.
reference: https://stackoverflow.com/questions/11994679/solving-2d-diffusion-heat-equation-with-cuda
https://cuda-tutorial.readthedocs.io/en/latest/tutorials/tutorial02/
https://stackoverflow.com/questions/7876624/timing-cuda-operations
*/


#include <stdio.h>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <fstream>
#include <iomanip>
#include <unistd.h>

#define BLOCK_XDIM 32
#define BLOCK_YDIM 32

//Jacobi update using cuda
__global__ void Jacobi_Iterator(const double * __restrict__ T_old, double * __restrict__ T_new, const int N)
{
	const int i = blockIdx.x * blockDim.x + threadIdx.x;
	const int j = blockIdx.y * blockDim.y + threadIdx.y;

	int P = i + j * N;           // node (i,j)
	int up = i + (j + 1)*N;       // node (i,j+1)
	int down = i + (j - 1)*N;       // node (i,j-1)
	int right = (i + 1) + j * N;       // node (i+1,j) 
	int left = (i - 1) + j * N;       // node (i-1,j) 

	if (i > 0 && i < N - 1 && j>0 && j < N - 1) {
		T_new[P] = 0.25 * (T_old[right] + T_old[left] + T_old[up] + T_old[down]);
	}
}

//initialize of plate temperature
void Initialize(double * __restrict host_temperature, const int N)
{
	for (int j = 0; j < N; j++) {
		for (int i = 0;i < N; i ++){
			host_temperature[j * N + i] = 20.0;
		}
	}
	for ( int i = 0; i < N; i ++) {
		if (i > 0.3*(N-1) && i < 0.7*(N-1)) {
			host_temperature[i] = 100.0;
		}
	}
}

int main(int argc, char* argv[])
{
    int N;  
	int total_iteration;
	int c; 
	const char* optstring = "N:I:";
    while ((c = getopt(argc, argv, optstring)) != -1)
    {
        switch (c) {
        case 'N':
            //printf("opt is N, N is: %s\n", optarg);
            N = atoi(optarg) + 2;
			if (N <= 0 || !N){
				printf("Invalid parameters, please check your values.");
			}
            break;
        case 'I':
            //printf("opt is I, I is: %s\n", optarg);
            total_iteration = atoi(optarg);
			if (total_iteration <= 0 || !total_iteration){
				printf("Invalid parameters, please check your values.");
			}
            break;
        case '?':
            printf("error optopt: %c\n", optopt);
            printf("error opterr: %d\n", opterr);
            break;
        }
    }

	double *host_temperature = (double *)calloc(N * N, sizeof(double));
	double *host_temperature_old = (double *)calloc(N * N, sizeof(double));
	Initialize(host_temperature, N);
	Initialize(host_temperature_old, N);
	double *host_temperature_GPU_result = (double *)malloc(N * N * sizeof(double));

	double *gpu_temperature;     
	cudaMalloc((void**)&gpu_temperature, N * N * sizeof(double));
	double *gpu_temperature_old; 
	cudaMalloc((void**)&gpu_temperature_old, N * N * sizeof(double));

	cudaMemcpy(gpu_temperature, host_temperature, N * N * sizeof(double), cudaMemcpyHostToDevice);
	cudaMemcpy(gpu_temperature_old, gpu_temperature, N * N * sizeof(double), cudaMemcpyDeviceToDevice);

	dim3 dimBlock(BLOCK_XDIM, BLOCK_YDIM);
	dim3 dimGrid((N + BLOCK_XDIM) / BLOCK_XDIM, (N + BLOCK_YDIM) / BLOCK_YDIM);

	float time;
	cudaEvent_t start, stop;

	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);

	for (int i = 0; i < total_iteration; i = i + 2) {
		Jacobi_Iterator << <dimGrid, dimBlock, 0 >> > (gpu_temperature, gpu_temperature_old, N);   // --- Update gpu_temperature_old     starting from data stored in gpu_temperature
		Jacobi_Iterator << <dimGrid, dimBlock, 0 >> > (gpu_temperature_old, gpu_temperature, N);   // --- Update gpu_temperature         starting from data stored in gpu_temperature_old
	}

	cudaMemcpy(host_temperature_GPU_result, gpu_temperature, N * N * sizeof(double), cudaMemcpyDeviceToHost);

	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&time, start, stop);

	printf("Time to generate:  %.2lf ms \n", time);
	
	std::ofstream outFile;
	outFile.open("finalTemperatures.csv");

	outFile.setf(std::ios::fixed);

	for (int j = 0; j < N; j ++){
		for (int i = 0; i < N; i ++){
			outFile <<std::setprecision(8) << host_temperature_GPU_result[j * N + i] << ',';
		}
		outFile << '\n';
	}
	
	free(host_temperature);
	free(host_temperature_GPU_result);

	cudaFree(gpu_temperature);
	cudaFree(gpu_temperature_old);

	return 0;
}