// File: gc.c
// Date: 2015. 07. 29.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#include "wafsim.h"

void gc_check(void)
{
	int i;
	int ret;
	long long n_empty_superblock;

	for(i=0; i<N_CORES; i++){
		n_empty_superblock = cores[i].n_empty_superblock;
		if(n_empty_superblock < 2){
//TEMP
//			print_block_state(i);

			garbage_collection(i);
		}
	}
}

void garbage_collection(int core_index)
{
	long long victim_superblock;
	int ret;
	int i;

	ret = select_victim_superblock(core_index, &victim_superblock);
	if(ret == FAIL){
		printf("Error[%s] select victim fail\n", __FUNCTION__);
		return;
	}

	copy_valid_pages(core_index, victim_superblock);


	/* Update core & superblock struct */
	cores[core_index].superblocks[victim_superblock].n_valid_pages = 0;
        cores[core_index].superblocks[victim_superblock].state = EMPTY;
        cores[core_index].superblocks[victim_superblock].cur_block = 0;
        cores[core_index].n_empty_superblock += 1;

//TEMP
//	printf("[%s] %lld of %d is empty, now.\n", __FUNCTION__, victim_superblock, core_index);

}

int select_victim_superblock(int core_index, long long* victim_superblock)
{
	int i;
	int n_valid_pages = PAGES_PER_SUPERBLOCK;
	int temp;
	long long victim;

	/* Select vitcim superblock by greedy */
	for(i=0; i<N_SUPERBLOCKS; i++){

		if(cores[core_index].superblocks[i].state != VICTIM){
			continue;
		}

		temp = cores[core_index].superblocks[i].n_valid_pages;
		if(temp < n_valid_pages){
			n_valid_pages = temp;
			victim = i;
		}
	}	

	if(n_valid_pages == PAGES_PER_SUPERBLOCK){
		printf("Error[%s] victim superblock is full with valid pages\n", __FUNCTION__);
		return FAIL;
	}

	*victim_superblock = victim;

	return SUCCESS;
}

void copy_valid_pages(int core_index, long long victim_superblock)
{
	int ret;
	int i, j;
	long long old_ppn, new_ppn;
	long long lpn;

	char* valid_array;

	int cur_block;

	for(i=0; i<BLOCKS_PER_SUPERBLOCK; i++){

		valid_array = cores[core_index].superblocks[victim_superblock].blocks[i].valid_array;

		for(j=0; j<PAGES_PER_BLOCK; j++){

			if(valid_array[j] == 'V'){

				old_ppn = core_index*PAGES_PER_CORE + i*PAGES_PER_FLASH + victim_superblock*PAGES_PER_BLOCK + j; 

				lpn = get_inverse_mapping(old_ppn);

				/* Get new empty page to write */
//TEMP
				if(lpn == -1){
					printf("Error[%s] lpn is -1 (old_ppn %lld)\n", __FUNCTION__, old_ppn);
				}

				ret = get_new_page(core_index, lpn, &new_ppn);
				if(ret == FAIL){
					printf("Error[%s] get new page return fail\n", __FUNCTION__);
					return;
				}

				/* Write the page to nand*/
				ssd_page_write(new_ppn, GC_WRITE);

				/* Update mapping & inverse mapping table */
				update_valid_array(old_ppn, INVALID);
				update_inverse_mapping_table(old_ppn, -1);

				update_mapping_table(lpn, new_ppn);
				update_inverse_mapping_table(new_ppn, lpn);
				update_valid_array(new_ppn, VALID);
			}
		}

		memset(cores[core_index].superblocks[victim_superblock].blocks[i].valid_array, 0, PAGES_PER_BLOCK);
                cores[core_index].superblocks[victim_superblock].blocks[i].n_valid_pages = 0;
                cores[core_index].superblocks[victim_superblock].blocks[i].cur_page = 0;
	}
}
