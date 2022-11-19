#include<iostream>
#include<fstream>
#include<time.h>
using namespace std;

double dRand(double, double);

int main(){
	srand(time(0));
	ofstream outdata;
	int i, n;
	
	outdata.open("input.txt");
	cout << "enter the size of the array: ";
	cin >> n;
	outdata << n << "\n";
	
	for(i=0; i<n; i++){
		outdata << dRand(0, 1);
		outdata << " ";
		outdata << dRand(0, 1);
		outdata << "\n";
	}
		
	return 0;
}

double dRand(double fMin, double fMax){
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax-fMin);
}
