#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/time.h>

void *parallel_function(void*);
void swap(float*, int, int);
void swap_row(float*, int, int);
void swap_row_range(float*, int, int);
float abs_v(float);
int n;
float *a;
float *l;
float *u;
int total_threads, n_of_threads, optimal_thread;

struct arg{
	int k;
	int start;
};
typedef struct arg *ARG;

int main(int argc, char *argv[]){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	n = atoi(argv[1]);
	total_threads = atoi(argv[2]);
	n_of_threads = atoi(argv[3]);
	
	optimal_thread = (n_of_threads <= 4)? n_of_threads : 4;
	
	int i, j, k, k1, c, sign;
	char s[10], *cp;
	float max;
	
	float *A = (float*)malloc(n*n*sizeof(float));
	float *L = (float*)malloc(n*n*sizeof(float));
	float *U = (float*)malloc(n*n*sizeof(float));
	float *p = (float*)malloc(n*sizeof(float));
	
	a = A;
	l = L;
	u = U;
	
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
		pthread_t tid[total_threads];
		ARG args[total_threads];
		
		for(i=0; i<total_threads; i++)
			args[i] = (ARG)malloc(sizeof(struct arg));
		
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
		
		for(i=0; i<total_threads; i++){
			args[i]->start = k+(i+1);
			args[i]->k = k;
		}
		
		for(i=0; i<total_threads; i++)
			pthread_create(&tid[i], NULL, parallel_function, args[i]);
		
		for(i=0; i<total_threads; i++)
			pthread_join(tid[i], NULL);
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
	if(total_threads == optimal_thread)
		fprintf(fp, "%.3f\t", time);
	fclose(fp);
	
	FILE *fp2 = fopen("pthread.txt", "a");
	if(total_threads == 1)
		fprintf(fp2, "%d\t%.3f\t", n, time);
	else if(total_threads == n_of_threads)
		fprintf(fp2, "%.3f\n", time);
	else
		fprintf(fp2, "%.3f\t", time);
	fclose(fp2);
		
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

void *parallel_function(void *parameter){
	ARG args = (ARG)parameter;
	int i, j, k;
	k = args->k;
	for(i=args->start; i<n; i+=total_threads){
		for(j=k+1; j<n; j++)
			*(a + i*n + j) -= *(l + i*n + k) * *(u + k*n + j);
	}
	pthread_exit(0);
}
