#include<iostream>
#include<fstream>
#include<omp.h>
using namespace std;

int main(){
	ifstream fin("input.txt");
	
	int i, j, k, n, limit;
	double difference;
	fin >> n;
	
	double A[n][n], A_dash[n][n];
	double b[n];
	double x[n], x_dash[n];
	
		
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			fin >> A[i][j];
			
	for(i=0; i<n; i++){
		fin >> b[i];
		x[i] = x_dash[i] = 0;	
	}
		
	cout << "Matrix_A:" << endl;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			cout << A[i][j] << " ";
		
		cout << endl;
	}
	cout << endl;
	
	cout << "Vector_b:" << endl;
	for(i=0; i<n; i++)
		cout << b[i] << " ";
	cout << endl << endl;	
	
	#pragma omp parallel for
		for(int i=0; i<n; i++){
			int j, k;
			for(j=0,k=0; k<n; j++,k++){
				if(j==i && j==n-1)
					break;
				else if(j == i)
					k++;
				
				A_dash[i][j] = -1 * A[i][k] / A[i][i];
			}
		
			A_dash[i][j] = b[i] / A[i][i];
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
	}	
	
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
