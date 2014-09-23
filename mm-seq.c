#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>

#define l1_CACHE 32*1024*1024

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif


typedef struct
{
	int size;
	float ** element;
} matrix;

/** 
 * Returns the number of nanoseconds since the beginning of the program
 * Requires linking with librt (-lrt flag on GCC)
 **/
long long wall_clock_time()
{
#ifdef __linux__
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
#else
	#warning "Your timer resoultion might be too low. Compile on Linux and link with librt"
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)(tv.tv_usec * 1000 + (long long)tv.tv_sec * 1000000000ll);
#endif
}

/**
 * Allocates memory for a matrix of size SIZE
 * The memory is allocated row-major order, i.e. 
 *  elements from the same row are allocated at contiguous 
 *  memory addresses.
 **/
void allocate_matrix(matrix* m, int size)
{
	int i, j;
	m->size = size;
	
	// allocate array for all the rows
	m->element = (float**)malloc(sizeof(float*) * size);
	if (m->element == NULL)
	{
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}
	
	// allocate an array for each row of the matrix
	for	(i = 0; i < size; i++)
	{
		m->element[i] = (float*)malloc(sizeof(float) * size);
		if (m->element[i] == NULL)
		{
			fprintf(stderr, "Out of memory\n");
			exit(1);
		}
	}
}

/**
 * Initializes the elements of the matrix with
 * random values between 0 and 9
 **/
void init_matrix(matrix m)
{
	int i, j;
	int size = m.size;
	
	
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
		{
			m.element[i][j] = rand() % 10;
		}
}

/**
 * Initializes the elements of the matrix with
 * element 0.
 **/
void init_matrix_zero(matrix m)
{
	int i, j;
	int size = m.size;
	
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
		{
			m.element[i][j] = 0.0;
		}
}


/** 
 * Frees the memory of matrix @m
 **/
void free_matrix(matrix m)
{
	int i;
	int size = m.size;
	
	for (i = 0; i < size; i++)
		free(m.element[i]);

	free(m.element);
}


/**
 * Multiplies matrix @a with matrix @b storing
 * the result1 in matrix @result1
 * 
 * The multiplication algorithm is the O(n^3) 
 * algorithm
 */
void mm(matrix a, matrix b, matrix result)
{
	int i, j, k;
	int size = a.size;
	long long before, after;

	before = wall_clock_time();
	// Do the multiplication
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			for(k = 0; k < size; k++)
				result.element[i][j] += a.element[i][k] * b.element[k][j];
    
	after = wall_clock_time();
	fprintf(stderr, "Matrix multiplication took %1.2f seconds\n", ((float)(after - before))/1000000000);
}

void mm_fast(matrix a, matrix b, matrix result)
{
	int i, j, k, temp;
	int size = a.size;
	long long before, after;

	before = wall_clock_time();
	// Do the multiplication
	for (k = 0; k < size; k++) {
		for (i = 0; i < size; i++) {
			temp = a.element[i][k] ;
			for(j = 0; j < size; j++)
				result.element[i][j] += temp * b.element[k][j];
    			}
	}
	after = wall_clock_time();
	fprintf(stderr, "Matrix multiplication took %1.2f seconds\n", ((float)(after - before))/1000000000);
}

void mm_block(matrix a, matrix b, matrix result)
{
	//printf("%d", min(1+2,2+4)) ; 
	int i, j, k, k_block, i_block, j_block, temp, k_limit, i_limit, j_limit;
	int size = a.size;
	long long before, after;
	int step = sqrt(l1_CACHE/3)/sizeof(float) ;
	//int step = 4;
	//printf("%d", step) ;

	before = wall_clock_time();
	// Do the multiplication
	for (k_block = 0; k_block < size; k_block += step) {
		k_limit = min(k_block + step, size);
		for (i_block = 0; i_block < size; i_block += step) {
			i_limit = min(i_block + step, size);
			for(j_block = 0; j_block < size; j_block += step) {
				j_limit = min(j_block + step, size);
				for (k = k_block; k < k_limit; k++) {
					for (i = i_block; i < i_limit; i++) {
						temp = a.element[i][k] ;
						for(j = j_block; j < j_limit; j++)
							result.element[i][j] += temp * b.element[k][j];
					}
				}
			}
		}
	}
    
	after = wall_clock_time();
	fprintf(stderr, "Matrix multiplication took %1.2f seconds\n", ((float)(after - before))/1000000000);

}	


void print_matrix(matrix m)
{
	int i, j;
	int size = m.size;
	
	for (i = 0; i < size; i++)
	{
		printf("row =%4d: ", i);
		for (j = 0; j < size; j++)
			printf("%6.2f  ", m.element[i][j]);
		printf("\n");
	}
}

int compare_matrix(matrix a, matrix b) 
{
	int i, j, size = a.size ;

	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			if (fabs(a.element[i][j] - b.element[i][j]) >= 0.000001)
				return 0;
	return 1 ;
}

void work(int size)
{
	matrix a, b, result1, result2, result3;

	// Allocate memory for matrices
	allocate_matrix(&a, size);
	allocate_matrix(&b, size);
	allocate_matrix(&result1, size);
	//allocate_matrix(&result2, size) ;
	allocate_matrix(&result3, size) ;
	
	// Initialize matrix elements
	init_matrix(a);
	init_matrix(b);
	init_matrix_zero(result1);


	// Perform sequential matrix multiplication
	mm(a, b, result1);

	//init_matrix_zero(result2);
	//mm_fast(a, b, result2) ;
	//if (compare_matrix(result1, result2)==1)
	//	printf("true") ;
	//free_matrix(result2);

	init_matrix_zero(result3);
	mm_block(a, b, result3) ;



	if (compare_matrix(result1, result3)==1)
		printf("true") ;

	// Print the result1 matrix
//	print_matrix(result1);
	
	free_matrix(a);
	free_matrix(b);

	free_matrix(result1);
	free_matrix(result3);
}


int main(int argc, char ** argv)
{
	int size;
	srand(0);
    
	if (argc >= 2)
		size = atoi(argv[1]);
	else
		size = 1024;
		
	fprintf(stderr,"Sequential matrix multiplication of size %d\n", size);
    
	// Multiply the matrices
	work(size);

	return 0;
}

