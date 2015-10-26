// File: comp_gc_raw2.c 
// Date: 2015. 07. 10.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2015
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define __USE_GNU
#include <fcntl.h>

#define W 32
#define R 32
#define M1 3
#define M2 24
#define M3 10

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define Identity(v) (v)

#define V0            STATE[state_i                   ]
#define VM1           STATE[(state_i+M1) & 0x0000001fU]
#define VM2           STATE[(state_i+M2) & 0x0000001fU]
#define VM3           STATE[(state_i+M3) & 0x0000001fU]
#define VRm1          STATE[(state_i+31) & 0x0000001fU]
#define newV0         STATE[(state_i+31) & 0x0000001fU]
#define newV1         STATE[state_i                   ]

#define FACT 2.32830643653869628906e-10

#define PAGE_SIZE	8192
#define SECTOR_SIZE 	512
#define SECTORS_PER_PAGE	((PAGE_SIZE)/(SECTOR_SIZE))

static unsigned int state_i = 0;
static unsigned int STATE[R];
static unsigned int z0, z1, z2; 

void make_random_offset(void);

void InitWELLRNG1024a (unsigned int *init){
   int j;
   state_i = 0;
   for (j = 0; j < R; j++)
     STATE[j] = init[j];
}

double WELLRNG1024a (void){
  z0    = VRm1;
  z1    = Identity(V0)       ^ MAT0POS (8, VM1);
  z2    = MAT0NEG (-19, VM2) ^ MAT0NEG(-14,VM3);
  newV1 = z1                 ^ z2; 
  newV0 = MAT0NEG (-11,z0)   ^ MAT0NEG(-7,z1)    ^ MAT0NEG(-13,z2) ;
  state_i = (state_i + 31) & 0x0000001fU;
  return ((double) STATE[state_i]  * FACT);
}

long long *recnum = NULL;
long long num_ios;
long long num_recs;

long long device_size_1k;
long long record_size_1k;
long long total_size_1k;
long long device_size;
long long record_size;
long long total_size;

int fd;
int iter;

long long get_current_utime(void)
{
        struct timeval current;

        gettimeofday(&current,NULL);

        return (current.tv_sec*1000000 + current.tv_usec);
}

/*
 * ./program_name  device_size_1k  record_size_1k 
 *
 */
int main(int argc, char* argv[])
{
	double iops = 0;

	/* Get arguments */
	device_size_1k 		= atoi(argv[1]);	// device size in Kbyte
	record_size_1k		= atoi(argv[2]);	// record size in Kbyte
	iter			= atoi(argv[3]);	// iteration

	/* Set Arguments */
	total_size_1k = device_size_1k * iter;
	device_size = device_size_1k * 1024;
	record_size = record_size_1k * 1024;
	total_size = total_size_1k * 1024;

	num_ios = (total_size_1k / record_size_1k);
	num_recs = (device_size_1k / record_size_1k);

	/* Allocate array for saving random offset numbers */
        recnum = (long long *)malloc(sizeof(*recnum)*num_ios);

	make_random_offset();

	return 0;
}

void make_random_offset(void)
{
	int i;
	int j;
	int index;

	long long r;
	unsigned int test[32];
	int length = (int)record_size / SECTOR_SIZE;
	

	srand(time(NULL));

	for(j = 0; j < iter; j++){

		*test = rand();
		InitWELLRNG1024a(test);

	        for(i = 0; i < num_recs; i++) { // re-shuffle the array

			r = (long long) (WELLRNG1024a() * (double)num_recs);

			index = num_recs * j + i;
			recnum[index] = r * record_size / SECTOR_SIZE;
	
//			printf("%lld\t%d\tW\n", recnum[index], length);
        	}
	}

	for(i = 0; i < num_ios; i++) {
		printf("%lld\t%d\tW\n", recnum[i], length);
        }
}
