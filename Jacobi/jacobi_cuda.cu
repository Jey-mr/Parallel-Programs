#include <iostream>
#include <fstream>
#include <sys/time.h>
using namespace std;

/*
__device__ double atomicAddUD(double* address, double val)
{
    unsigned long long int* address_as_ull =
                              (unsigned long long int*)address;
    unsigned long long int old = *address_as_ull, assumed;

    do {
        assumed = old;
        old = atomicCAS(address_as_ull, assumed,
                        __double_as_longlong(val +
                               __longlong_as_double(assumed)));

     } while (assumed != old);

    return __longlong_as_double(old);
}
*/

__global__ void mykernal(double *dA1, double *dx, double *dx1, int n){
	int i, j, k;
	
	i = threadIdx.x;
	for(j=0,k=0; k<n; j++,k++){
		if(j==i && j==n-1)
			break;
		else if(j == i)
			k++;
				
		dx1[i] += (*(dA1 + i*n + j) * dx[k]);
		//dx1[i] += (dA1[i][j] * dx[k]);
	}
		
	dx1[i] += *(dA1 + i*n + n-1);	
	//dx1[i] += dA1[i][n-1];
}

__global__ void mykernal2(double *dx, double *dx1, double *dxp){
	int i = threadIdx.x;
	
	dxp[i] = dx[i];
	dx[i] = dx1[i];
	dx1[i] = 0;
}


int main(){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	ifstream fin("input.txt");
	
	int i, j, k, n, l, limit;
	double difference;
	fin >> n;
	
	double *A = (double*)malloc(sizeof(double)*n*n); 
	double *A_dash = (double*)malloc(sizeof(double)*n*n);
	double *b = (double*)malloc(sizeof(double)*n); 
	double *x = (double*)malloc(sizeof(double)*n); 
	double *x_dash = (double*)malloc(sizeof(double)*n); 
	
		
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			fin >> *(A + i*n + j);
			
	for(i=0; i<n; i++){
		fin >> b[i];
		x[i] = x_dash[i] = 0;	
	}
		
/*	cout << "Matrix_A:" << endl;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			cout << *(A + i*n + j) << " ";
		
		cout << endl;
	}
	cout << endl;
	
	cout << "Vector_b:" << endl;
	for(i=0; i<n; i++)
		cout << b[i] << " ";
	cout << endl << endl;	*/
	
	for(i=0; i<n; i++){
		for(j=0,k=0; k<n; j++,k++){
			if(j==i && j==n-1)
				break;
			else if(j == i)
				k++;
			
			*(A_dash + i*n + j) = -1 * *(A + i*n + k) / *(A + i*n + i);				
		}
		
		*(A_dash + i*n + j) = b[i] / *(A + i*n + i);
	}
	
/*	cout << "Matrix_A_dash:" << endl;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			cout << A_dash[i][j] << " ";
		
		cout << endl;
	}
	cout << endl;	*/
	
	double *dA1, *dx, *dx1, *dxp;
	
	cudaMalloc((void**)&dA1, sizeof(double)*n*n);
	cudaMalloc((void**)&dx, sizeof(double)*n);
	cudaMalloc((void**)&dx1, sizeof(double)*n);
	cudaMalloc((void**)&dxp, sizeof(double)*n);
	
	cudaMemcpy(dA1, A_dash, sizeof(double)*n*n, cudaMemcpyHostToDevice);
	cudaMemcpy(dx, x, sizeof(double)*n, cudaMemcpyHostToDevice);
	cudaMemcpy(dx1, x_dash, sizeof(double)*n, cudaMemcpyHostToDevice);
	cudaMemcpy(dxp, x_dash, sizeof(double)*n, cudaMemcpyHostToDevice);
	
	limit = 5*n*n;
	for(l=0; l<limit; l++){
		mykernal<<<1, n>>>(dA1, dx, dx1, n);
		cudaDeviceSynchronize();
		
		mykernal2<<<1, n>>>(dx, dx1, dxp);
		cudaDeviceSynchronize();	
	}
	
	cudaMemcpy(x, dx, sizeof(double)*n, cudaMemcpyDeviceToHost);
	cudaMemcpy(x_dash, dxp, sizeof(double)*n, cudaMemcpyDeviceToHost);
	cudaFree(dA1);
	cudaFree(dx);
	cudaFree(dx1);	
	cudaFree(dxp);
	
	difference = 0;
	for(i=0; i<n; i++)
		difference += abs(abs(x[i]) - abs(x_dash[i]));
	
	//cout << "Difference: " << difference << endl;
	
/*	if(difference/(5*n*n) > 1)
		cout << "it will not converge" << endl;	
	else{
		cout << "Vector_x:" << endl;
		
		for(i=0; i<n; i++)
			cout << x[i] << " ";
		cout << endl << endl;
	}	*/
	
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;
	double time = (double)micros / 1000000; 
	
	cout << "Cuda: " << time << endl;
	
	return 0;
}
