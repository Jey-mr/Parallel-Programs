#include<iostream>
#include<fstream>
#include<pthread.h>
using namespace std;

double *A, *A_dash, *b, *x, *x_dash;
int n, threads;
pthread_barrier_t barrier;

void *assignment(void*);
void *convergence(void*);

int main(){
	ifstream fin("input.txt");
	
	int i, j, k, limit;
	double difference;
	fin >> n;
	
	A = (double*)malloc(sizeof(double)*n*n);
	A_dash = (double*)malloc(sizeof(double)*n*n);
	
	b = (double*)malloc(sizeof(double)*n);
	x = (double*)malloc(sizeof(double)*n);
	x_dash = (double*)malloc(sizeof(double)*n);
		
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
	
	threads = 8;
	pthread_t t[threads];
	pthread_barrier_init(&barrier, NULL, threads);
	
	for(i=0; i<threads; i++)
		pthread_create(&t[i], NULL, assignment, &i);
		
	for(i=0; i<threads; i++)
		pthread_join(t[i], NULL);
	
/*	#pragma omp parallel for
		for(int i=0; i<n; i++){
			int j, k;
			for(j=0,k=0; k<n; j++,k++){
				if(j==i && j==n-1)
					break;
				else if(j == i)
					k++;
				
				*(A_dash + i*n + j) = -1 * A[i][k] / A[i][i];
				//A_dash[i][j] = -1 * A[i][k] / A[i][i];
			}
		
			*(A_dash + i*n + j) = b[i] / A[i][i];
		}	*/
	
	cout << "Matrix_A_dash:" << endl;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			cout << *(A_dash + i*n + j) << " ";
		
		cout << endl;
	}
	cout << endl;	
	
/*	limit = 5*n*n;
	for(int l=0; l<limit; l++){
		#pragma omp parallel for
		for(int i=0; i<n; i++){
			int j, k;
			
			for(j=0,k=0; k<n; j++,k++){
				if(j==i && j==n-1)
					break;
				else if(j == i)
					k++;
				
				x_dash[i] += (A_dash[i][j] * x[k]);
			}
			
			x_dash[i] += A_dash[i][n-1];
		}
		
		difference = 0;
		for(int i=0; i<n; i++)
			difference += abs(abs(x[i]) - abs(x_dash[i]));
		
		#pragma omp parallel for
		for(int i=0; i<n; i++){
			x[i] = x_dash[i];
			x_dash[i] = 0;
		}
	}	*/
	
	//cout << "Difference: " << difference << endl;
	
	difference = 2;
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

void *assignment(void *p){
	int *start = (int*)p;
	
	for(int i=*start; i<n; i+=threads){
		int j, k;
		for(j=0,k=0; k<n; j++,k++){
			if(j==i && j==n-1)
				break;
			else if(j == i)
				k++;
				
			*(A_dash + i*n + j) = -1 * *(A + i*n + k) / *(A + i*n + i);
			//A_dash[i][j] = -1 * A[i][k] / A[i][i];
		}
		
		*(A_dash + i*n + j) = b[i] / *(A + i*n + i);
	}
	
	pthread_barrier_wait(&barrier);
	pthread_exit(0);
}
