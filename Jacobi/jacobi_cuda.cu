#include<iostream>
#include<fstream>
using namespace std;

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

__global__ void mykernal(double *dA1, double *dx, double *dx1, int n){
	int i, j;
	double value;
	i = blockIdx.x;
	j = threadIdx.x;
	
	if(j < i)
		value = *(dA1 + i*n + j) * dx[j];
	else if(j < n-1)
		value = *(dA1 + i*n + j) * dx[j+1];
	else 
		value = *(dA1 + i*n + j);
		
	atomicAddUD(&dx1[i], value);
}

__global__ void mykernal2(double *dx, double *dx1, double *d){
	int i = threadIdx.x;
	double diff = abs(abs(dx[i]) - abs(dx1[i]));
	
	atomicAddUD(d, diff);
	
	dx[i] = dx1[i];
	dx1[i] = 0;
}


int main(){
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
		
	cout << "Matrix_A:" << endl;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			cout << *(A + i*n + j) << " ";
		
		cout << endl;
	}
	cout << endl;
	
	cout << "Vector_b:" << endl;
	for(i=0; i<n; i++)
		cout << b[i] << " ";
	cout << endl << endl;	
	
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
	
	double *dA1, *dx, *dx1, *d;
	
	cudaMalloc((void**)&dA1, sizeof(double)*n*n);
	cudaMalloc((void**)&dx, sizeof(double)*n);
	cudaMalloc((void**)&dx1, sizeof(double)*n);
	cudaMalloc((void**)&d, sizeof(double));
	
	cudaMemcpy(dA1, A_dash, sizeof(double)*n*n, cudaMemcpyHostToDevice);
	cudaMemcpy(dx, x, sizeof(double)*n, cudaMemcpyHostToDevice);
	cudaMemcpy(dx1, x_dash, sizeof(double)*n, cudaMemcpyHostToDevice);
	
	limit = 5*n*n;
	for(l=0; l<limit; l++){
		mykernal<<<n, n>>>(dA1, dx, dx1, n);
		cudaDeviceSynchronize();
		
		difference = 0;
		cudaMemcpy(d, &difference, sizeof(double), cudaMemcpyHostToDevice);
		mykernal2<<<1, n>>>(dx, dx1, d);
		cudaDeviceSynchronize();		
	}
	
	cudaMemcpy(x, dx, sizeof(double)*n, cudaMemcpyDeviceToHost);
	cudaMemcpy(&difference, d, sizeof(double), cudaMemcpyDeviceToHost);
	cudaFree(dA1);
	cudaFree(dx);
	cudaFree(dx1);	
	cudaFree(d);
	
	//cout << "Difference: " << difference << endl;
	
	if(difference > 1)
		cout << "it will not converge" << endl;	
	else{
		cout << "Vector_x:" << endl;
		
		for(i=0; i<n; i++)
			cout << x[i] << " ";
		cout << endl << endl;
	}
	
	return 0;
}
