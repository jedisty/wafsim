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

#define PAGE_SIZE	8192
#define SECTOR_SIZE 	512
#define SECTORS_PER_PAGE	((PAGE_SIZE)/(SECTOR_SIZE))

void make_random_offset(void);


#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

/* The array for the state vector */
static unsigned long long mt[NN];

/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1;

void init_genrand64(unsigned long long seed)
{
        mt[0] = seed;
        for (mti=1; mti<NN; mti++)
                mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

unsigned long long genrand64_int64(void)
{
        int i;
        unsigned long long x;
        static unsigned long long mag01[2]={0ULL, MATRIX_A};

        if (mti >= NN) { /* generate NN words at one time */

                /* if init_genrand64() has not been called, */
                /* a default initial seed is used     */
                if (mti == NN+1){
                        srand((unsigned)time(NULL));
                        init_genrand64((unsigned long long)rand());
                }

                for (i=0;i<NN-MM;i++) {
                        x = (mt[i]&UM)|(mt[i+1]&LM);
                        mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
                }
                for (;i<NN-1;i++) {
                        x = (mt[i]&UM)|(mt[i+1]&LM);
                        mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
                }
                x = (mt[NN-1]&UM)|(mt[0]&LM);
                mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

                mti = 0;
        }

        x = mt[mti++];

        x ^= (x >> 29) & 0x5555555555555555ULL;
        x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
        x ^= (x << 37) & 0xFFF7EEE000000000ULL;
        x ^= (x >> 43);

        return x;
}

long long *recnum = NULL;
long long *temp_recnum = NULL;
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
	iter			= atoi(argv[3]);

	/* Set Arguments */
	total_size_1k = device_size_1k * iter;
	device_size = device_size_1k * 1024;
	record_size = record_size_1k * 1024;
	total_size = total_size_1k * 1024;

	num_ios = (total_size_1k / record_size_1k);
	num_recs = (device_size_1k / record_size_1k);

	/* Allocate array for saving random offset numbers */
        temp_recnum = (long long *)malloc(sizeof(*temp_recnum)*num_recs);

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

	unsigned int test[32];
	long long tmp;
	unsigned long long big_rand;
	int length = (int)record_size / SECTOR_SIZE;

	for(j=0; j<iter; j++){
		/* Initializing the array of random numbers */
	        for(i = 0; i < num_recs; i++){
        	        temp_recnum[i] = i;
	        }
        	/* pre-compute random sequence based on 
	                Fischer-Yates (Knuth) card shuffle */
        	for(i = 0; i < num_recs; i++) {
	                big_rand=genrand64_int64();
                	big_rand = big_rand % num_recs;
        	        tmp = temp_recnum[i];
	                temp_recnum[i] = temp_recnum[big_rand];
                	temp_recnum[big_rand] = tmp;
        	}

	        for(i = 0; i < num_recs; i++) {
        	        temp_recnum[i] *= record_size / SECTOR_SIZE;
	        }

	        for(i = 0; i < num_recs; i++) {
			index = num_recs * j + i;
        	        recnum[index] = temp_recnum[i];
	        }
	}

        for(i = 0; i < num_ios; i++) {
		printf("%lld\t%d\tW\n", recnum[i], length);
        }
}
