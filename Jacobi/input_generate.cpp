#include<iostream>
#include<fstream>
#include <cstdlib>
#include <time.h>
using namespace std;

double fRand(double, double);
int pow(int, int);

int main(){
	ofstream outdata;
	outdata.open("input.txt");
	srand(time(0));
	
	int i, j, n;
	cout << "enter the size of the matrix(n*n):";
	cin >> n;
	outdata << n << "\n";
	double A[n][n], x[n], sum;
	
	//outdata << "\nMatrix_A:\n";
	for(i=0; i<n; i++){
		sum = 0;
		
		for(j=0; j<n; j++){
			if(i==j)
				continue;
				
			A[i][j] = fRand(1, 5) * pow(-1, rand()%10); 
			sum += abs(A[i][j]);
			//outdata << A[i][j] << " ";
		}
		
		A[i][i] = (sum + 1) * pow(-1, rand()%10);
			
		//outdata << "\n";
	}	
	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			outdata << A[i][j] << " "; 
		
		outdata << "\n";		
	}
	
	for(i=0; i<n; i++)
		x[i] = fRand(1, 5) * pow(-1, rand()%10);
		
	//outdata << "\nVector_b:\n";
	for(i=0; i<n; i++){
		sum = 0;
		
		for(j=0; j<n; j++)
			sum += (A[i][j] * x[j]);
			
		outdata << sum << " ";
	}
	
	outdata << "\n\nVector_X:\n";
	for(i=0; i<n; i++)
		outdata << x[i] << " ";
	
	return 0;
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int pow(int p, int n){
	if(n == 0)
		return 1;
	
	int result = 1;
	for(int i=0; i<n; i++)
		result *= p;
		
	return result;
}

