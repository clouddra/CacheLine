#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#define MAX_SIZE 64*1024*1024
#define ITERATE 1000000000

typedef struct
{
	int size;
	char * element;
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
	m->size = size;

	// allocate array for all the rows
	m->element = (char*)malloc(sizeof(char) * size);
	if (m->element == NULL)
	{
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

}

/** 
 * Frees the memory of matrix @m
 **/

void free_matrix(matrix m)
{
	free(m.element);
}

/**
 * Initializes the elements of the matrix with
 * random values between 0 and 9
 **/

void init_matrix(matrix m)
{
	int i;
	int size = m.size;
	
	for (i=0; i<m.size; i++)
		m.element[i] = 'a';		

}

void cache_line(matrix m)
{
	int i,j=0, k;
	int size = m.size;
	char temp ;
	long long before, after ;


	for (k=2; k<1024 ; k*=2) {  	
		before = wall_clock_time() ;		
		for (i = 0, j = 0; i < ITERATE; i++, j=(j+k)%size)
			m.element[j] = 'b';
		after = wall_clock_time() ;
		fprintf(stderr, "%d bytes jump - total: %1.2f seconds Average: %1.4f nanoseconds\n", k, (float)(after-before)/1000000000.0, (double)(after-before)/(ITERATE)) ;

	}

}



void cache_level(matrix m)
{
	int i,j=0, k;
	char temp ;
	int size = m.size;
	long long before, after ;
	matrix c ;
//	printf("%zu,%zu\n", sizeof temp, sizeof(char));

	for (k=1024; k<1024*1024 ; k*=2) {  
		allocate_matrix(&c, k) ;
		init_matrix(c) ;	
		//printf("%d\n", c.size) ;
		before = wall_clock_time() ;		
		for (i = 0, j = 0; i < ITERATE; j= (j+16)%c.size, i++) {
			c.element[j]++;
		}
		after = wall_clock_time() ;
		fprintf(stderr, "%d Kb took %1.2f microseconds\n", c.size/1024, ((float)(after - before))/1000000);
		free_matrix(c);
	}

/*
	for (k=1024; k<size ; k*=2) {  	
		before = wall_clock_time() ;		
		for (i = 0, j = 0; i < ITERATE; i++, j=(j+32)%k)
			m.element[j]++;
		after = wall_clock_time() ;
		fprintf(stderr, "%d kb - total: %1.2f seconds Average: %1.4f nanoseconds\n", k/1024, (float)(after-before)/1000000000.0, (double)(after-before)/(ITERATE)) ;

	}
*/

}


/*
void read_matrix(matrix m)
{
	int i,j=0, k;
	int size = m.size;
	char temp ;
	long long before, after ;
	


		before = wall_clock_time() ;		
		for (i=0; i<m.size; m++)
			temp = m.element[i];	
		after = wall_clock_time() ;
		fprintf(stderr, "%d bytes - Editing matrix total: %1.2f seconds Average: %1.4f nanoseconds\n", k, (float)(after-before)/1000000000.0, (double)(after-before)/(ITERATE)) ;






}
*/




/**
 * Multiplies matrix @a with matrix @b storing
 * the result in matrix @result
 * 
 * The multiplication algorithm is the O(n^3) 
 * algorithm
 */

void read_matrix(matrix m)
{
	int i;
	char j;
	int size = m.size;
	long long before, after ;

	before = wall_clock_time() ;
	for (i = 0; i < size; i++)
	{
		j = m.element[i] ;

	}
	after = wall_clock_time() ;
	fprintf(stderr, "Accessing matrix %1.2f seconds\n", (float)(after-before)/1000000000.0) ;

}



void work(int size)
{
	matrix a ;

	// Allocate memory for matrices
	allocate_matrix(&a, size);

	// Initialize matrix elements
	init_matrix(a);


	// Print the result matrix
//	cache_line(a) ;
//	cache_line(a) ;
//	init_matrix(a);	
//	read_matrix(a);
//	read_matrix(a) ;	

	
	cache_level(a) ;

	free_matrix(a);
}


int main(int argc, char ** argv)
{
	int size;
	srand(0);

	if (argc >= 2)
		size = atoi(argv[1]);
	else
		size = 64*1024*1024;

	fprintf(stderr,"Sequential matrix multiplication of size %d\n", size);

	// Multiply the matrices
	work(size);

	return 0;
}

