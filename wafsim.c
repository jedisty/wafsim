// File: wafsim.c
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved 

#include "wafsim.h"

long long n_written_pages;
long long n_gc_written_pages;
long long n_total_sectors;
double waf;

int main(int argc, char* argv[])
{
	FILE *i_fp;
	int ret;

	long long sector_nb;
	unsigned int length;
	char io_type;

	/* Init globla variable */
	init_wafsim();

	/* Open trace file */
	i_fp = fopen(argv[1], "r");
	if(i_fp == NULL){
		printf("[%s] trace file %s open fail\n", __FUNCTION__, argv[1]);
		return -1;
	}

	/* Read trace file and processing */
	while(1){
		/* Read a request */
		ret = fscanf(i_fp, "%lld\t%u\t%c\n", &sector_nb, &length, &io_type);

		/* If end of the trace file */
		if(ret < 0)
			break;

		/* Branch by io_type */
		if(io_type == 'W'){
			ftl_write(sector_nb, length);
		}
		else{
			printf("[%s]Wafsim only treats Write request\n", __FUNCTION__);
			break;
		}
	}

	show_result();

	print_block_state(0);

	return 0;	
}

void init_wafsim(void)
{
	n_written_pages = 0;
	n_gc_written_pages = 0;
	n_total_sectors = 0;
	waf = 0;

	SECTORS_PER_PAGE = PAGE_SIZE / SECTOR_SIZE;
	BLOCKS_PER_SUPERBLOCK = N_CHANNEL * N_WAY * N_PLAIN; 
	PAGES_PER_SUPERBLOCK = BLOCKS_PER_SUPERBLOCK * PAGES_PER_BLOCK;
	PAGES_PER_CORE = PAGES_PER_SUPERBLOCK * N_SUPERBLOCKS;
	PAGES_PER_FLASH = N_SUPERBLOCKS * PAGES_PER_BLOCK;
 
	BLOCK_SIZE = PAGE_SIZE * PAGES_PER_BLOCK;
	SUPERBLOCK_SIZE = BLOCK_SIZE * BLOCKS_PER_SUPERBLOCK;

	N_SECTORS = (SUPERBLOCK_SIZE * N_SUPERBLOCKS * N_CORES) / SECTOR_SIZE;
	N_PAGES = (SUPERBLOCK_SIZE * N_SUPERBLOCKS * N_CORES) / PAGE_SIZE;

	/* Init page table */
	init_mapping_table();
	init_inverse_mapping_table();
	init_cores();
}

void show_result(void)
{
	long long n_written_in_byte = n_written_pages * PAGE_SIZE;
	long long n_total_sectors_in_byte = n_total_sectors * SECTOR_SIZE;

	waf = (double)n_written_in_byte / n_total_sectors_in_byte;

	printf("\n== Simulation Result ==\n");
	printf("FS     --> DEVICE	: %lld byte\n", n_total_sectors_in_byte);
	printf("DEVICE --> NAND		: %lld byte\n", n_written_in_byte);
	printf("WAF			: %lf \n", waf);
}
