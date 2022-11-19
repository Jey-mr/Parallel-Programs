#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>

void swap(float*, int, int);
void swap_row(float*, int, int);
void swap_row_range(float*, int, int);
float abs_v(float);
int n;

int main(int argc, char *argv[]){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	n = atoi(argv[1]); 
	int i, j, k, k1, c, sign;
	char s[10], *cp;
	float max;
	float *a = (float*)malloc(n*n*sizeof(float));
	float *l = (float*)malloc(n*n*sizeof(float));
	float *u = (float*)malloc(n*n*sizeof(float));
	float *p = (float*)malloc(n*sizeof(float));
	
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			*(a + i*n + j) = rand() % 20;
	/*
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			cp = s;
			sign = 1;
			while((c = getchar()) != EOF){
				if(c == '-'){
					sign = -1;
					continue;
				}
				else if(isdigit(c) || c == '.')
					*cp++ = c;
				else if(c == ',')
					break;
			}
			*cp == '\0';
			*(a + i*n + j) = atof(s) * sign;
		}
	}
	
	printf("\nThe given matrix is\n");	
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			printf("%f ", *(a + i*n + j));
		printf("\n");
	}
	*/
	for(i=0; i<n; i++)
		for(j=0; j<n; j++){
			if(i == j)
				*(l + n*i+ j) = 1;
			else
				*(l + n*i+ j) = 0;
		}
		
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			*(u + n*i + j) = 0;
			
	for(i=0; i<n; i++)
		p[i] = i;
		
	for(k=0; k<n; k++){
		max = 0;
		for(i=k; i<n; i++){
			if(max < abs_v(*(a + i*n + k))){
				max = abs_v(*(a + i*n + k));
				k1 = i;
			}
		}
		if(max == 0){
			printf("error: singular matrix\n");
			exit(1);
		}
		swap(p, k, k1);
		swap_row(a, k, k1);
		swap_row_range(l, k, k1);
		*(u + k*n + k) = *(a + k*n + k);

		for(i=k+1; i<n; i++){
			*(l + i*n + k) = *(a + i*n + k) / *(u + k*n + k);
			*(u + k*n + i)  = *(a + k*n + i) ;
		}
		for(i=k+1; i<n; i++)
			for(j=k+1; j<n; j++)
				*(a + i*n + j) -= *(l + i*n + k) * *(u + k*n + j);
	}
	/*
	printf("\nthe L matrix is\n");
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			printf("%f ", *(l + n*i + j));
		printf("\n");
	}
	printf("\nthe U matrix is\n");
	for(i=0; i<n; i++){
		for(j=0; j<n; j++)
			printf("%f ", *(u + n*i + j));
		printf("\n");
	}
	printf("\nthe p vector is\n");
	for(i=0; i<n; i++)
		printf("%f ",p[i]);
	printf("\n");
	*/
	
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;
	double time = (double)micros / 1000000;
	
	//printf("\aCPU_TIME (in seconds): %.3f\n\n", time); 
	
	FILE *fp = fopen("output.txt", "a");
	fprintf(fp, "%d\t%.3f\t", n, time);
	fclose(fp);
	return 0;
}

void swap(float *p, int k, int k1){
	float temp;
	temp = p[k];
	p[k] = p[k1];
	p[k1] = temp;
}

void swap_row(float *a, int k, int k1){
	int i;
	float temp;
	for(i=0; i<n; i++){
		temp = *(a + k*n + i);
		*(a + k*n + i) = *(a + k1*n + i);
		*(a + k1*n + i) = temp;
	}
}

void swap_row_range(float *l, int k, int k1){
	int i;
	float temp;
	for(i=0; i<k; i++){
		temp = *(l + k*n + i);
		*(l + k*n + i) = *(l + k1*n + i);
		*(l + k1*n + i) = temp;
	}
}

float abs_v(float n){
	return (n < 0)? -n:n;
}
