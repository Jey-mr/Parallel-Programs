#include<stdio.h>

int main(){
	FILE *f1, *f2, *f3;
	f1 = fopen("output.txt", "w");
	f2 = fopen("pthread.txt", "w");
	f3 = fopen("openmp.txt", "w");
	
	fprintf(f1, "");
	fprintf(f2, "");
	fprintf(f3, "");
	
	fclose(f1);
	fclose(f2);
	fclose(f3);
	return 0;
}
