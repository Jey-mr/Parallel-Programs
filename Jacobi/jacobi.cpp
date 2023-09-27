#include <iostream>
#include <fstream>
#include <sys/time.h>
using namespace std;

int main(){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	ifstream fin("input.txt");
	
	int i, j, k, n, limit;
	double difference;
	fin >> n;
	
	//double A[n][n], A_dash[n][n];
	double *A = (double*)malloc(sizeof(double)*n*n);
	double *A_dash = (double*)malloc(sizeof(double)*n*n);
	double b[n];
	double x[n], x_dash[n];
	
		
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
			cout << A[i][j] << " ";
		
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
			//A_dash[i][j] = -1 * A[i][k] / A[i][i];
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
	
	limit = 5*n*n;
	for(int l=0; l<limit; l++){
		for(i=0; i<n; i++){
			for(j=0,k=0; k<n; j++,k++){
				if(j==i && j==n-1)
					break;
				else if(j == i)
					k++;
				
				x_dash[i] += (*(A_dash + i*n + j) * x[k]);
			}
			
			x_dash[i] += *(A_dash + i*n + n-1);
		}
		
		difference = 0;
		for(i=0; i<n; i++){
			difference += abs(abs(x[i]) - abs(x_dash[i]));
			x[i] = x_dash[i];
			x_dash[i] = 0;
		}
	}
	
	//cout << "Difference: " << difference << endl;
	
/*	if(difference > 1)
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
	
	cout << "Serial: " << time << endl;
	
	return 0;
}
