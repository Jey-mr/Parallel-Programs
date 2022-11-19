#include <iostream>
#include <mpi.h>
#include <pthread.h>
#include <fstream>
#include <cmath>
#include <time.h>
using namespace std;

int N, n, threads, elements_per_proc;
double range, *X, *Y;
pthread_mutex_t lock;

void create_rand_nums();
int compute_n(int*, int);
void *compute(void*);
ifstream fin("input.txt");

struct ARG{
	int index;
	double *x, *y;
};

int main(){
	clock_t begin = clock();
	srand(time(0));
	int i, s, Nc;
	double r, pi;
	fin >> N;
	
	s = 1;
	r = 0.5;
	range = pow(r, 2);
	pthread_mutex_init(&lock, NULL);
	
	MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    elements_per_proc = N / world_size;

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	float *rand_nums;
	if (world_rank == 0){
		create_rand_nums();
	}

	// Create a buffer that will hold a subset of the random numbers
	double *sub_x = (double*)malloc(sizeof(double) * elements_per_proc);
	double *sub_y = (double*)malloc(sizeof(double) * elements_per_proc);

	// Scatter the random numbers to all processes
	MPI_Scatter(X, elements_per_proc, MPI_DOUBLE, sub_x, elements_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(Y, elements_per_proc, MPI_DOUBLE, sub_y, elements_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	n = 0;
	threads = 4;
	pthread_t t[threads];
	ARG arguments[threads];
	
	for(int i=0; i<threads; i++){
		arguments[i].x = sub_x;
		arguments[i].y = sub_y;
		arguments[i].index = i;
		
		pthread_create(&t[i], NULL, compute, &arguments[i]);
	}
		
	for(int i=0; i<threads; i++)
		pthread_join(t[i], NULL);
	
	int *sub_n = NULL;
	if (world_rank == 0) {
		sub_n = (int*)malloc(sizeof(int) * world_size);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(&n, 1, MPI_INT, sub_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (world_rank == 0) {
		Nc = compute_n(sub_n, world_size);
		cout << "the value of N: " << N << endl;
		cout << "the value of Nc: " << Nc << endl;
	
		pi = Nc / (N * range);
		cout << "the value of pi: " << pi << endl;
		//cout << "total time taken: "<< (double)(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl;
		
		free(X);
		free(Y);
	}

	pthread_mutex_destroy(&lock);
	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

void *compute(void *p){
	ARG *arg = (ARG*)p;
	double *sub_x = arg->x;
	double *sub_y = arg->y;
	int start = arg->index;

	for(int i=start; i<elements_per_proc; i+=threads){
		double a = pow(sub_x[i]-0.5, 2);
		double b = pow(sub_y[i]-0.5, 2);		
		double d = sqrt(a + b);
		
		if(d <= range){
			pthread_mutex_lock(&lock);
			n++;
			pthread_mutex_unlock(&lock);
		}
	}	
	
	pthread_exit(0);
}

void create_rand_nums(){
	X = (double*)malloc(sizeof(double) * N);
	Y = (double*)malloc(sizeof(double) * N);
	
	for(int i=0; i<N; i++){
		fin >> X[i];
		fin >> Y[i];
	}
}

int compute_n(int *sub_n, int world_size){
	int n = 0;
	
	for(int i=0; i<world_size; i++)	
		n += sub_n[i];
		
	return n;
}
