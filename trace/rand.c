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

void make_random_offset(void);

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
	int length = (int)record_size / SECTOR_SIZE;

	for(j = 0; j < iter; j++){

		srand(time(NULL));

	        for(i = 0; i < num_recs; i++) { // re-shuffle the array

			index = num_recs * j + i;
			recnum[index] = rand()%num_recs;
	
//			printf("%lld\t%d\tW\n", recnum[index], length);
        	}

		sleep(2);
	}

	for(i = 0; i < num_ios; i++) {
		recnum[i] *= length;
                printf("%lld\t%d\tW\n", recnum[i], length);
        }
}
