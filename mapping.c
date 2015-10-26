// File: mapping.c
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#include "wafsim.h"

struct core* cores;
long long* mapping_table;
long long* inverse_mapping_table;

void init_mapping_table(void)
{
	int i;

	mapping_table = (long long*)calloc(N_PAGES, sizeof(long long));
	if(mapping_table == NULL){
		printf("Error[%s] Calloc mapping table fail\n", __FUNCTION__);
		return;
	}

	for(i=0; i<N_PAGES; i++){
		mapping_table[i] = -1;
	}
}

void init_inverse_mapping_table(void)
{
	int i;

        inverse_mapping_table = (long long*)calloc(N_PAGES, sizeof(long long));
        if(inverse_mapping_table == NULL){
                printf("Error[%s] Calloc mapping table fail\n", __FUNCTION__);
                return;
        }

        for(i=0; i<N_PAGES; i++){
                inverse_mapping_table[i] = -1;
        }
}

void init_cores(void)
{
	int i, j, k;

	cores = (core*)calloc(N_CORES, sizeof(core));
	if(cores == NULL){
		printf("Error[%s] Calloc cores fail\n", __FUNCTION__);
		return;
	}

	/* Init each cores*/
	for(i=0; i<N_CORES; i++){
		cores[i].superblocks = (superblock*)calloc(N_SUPERBLOCKS, sizeof(superblock));
		if(cores[i].superblocks == NULL){
			printf("Error[%s] Calloc superblocks fail\n", __FUNCTION__);
			goto fail;
		}
		else{
			cores[i].n_empty_superblock = N_SUPERBLOCKS;
			cores[i].cur_superblock = 0;
		}
	}

	/* Init superblocks */
	for(i=0; i<N_CORES; i++){
		for(j=0; j<N_SUPERBLOCKS; j++){
			cores[i].superblocks[j].blocks = (block*)calloc(BLOCKS_PER_SUPERBLOCK, sizeof(block));
			if(cores[i].superblocks[j].blocks == NULL){
				printf("Error[%s] Calloc blocks fail\n", __FUNCTION__);
				goto fail;
			}
			else{
				cores[i].superblocks[j].n_valid_pages = 0;
				cores[i].superblocks[j].state = EMPTY;
				cores[i].superblocks[j].cur_block = 0;
			}
		}
	}

	/* Init blocks */
	for(i=0; i<N_CORES; i++){
		for(j=0; j<N_SUPERBLOCKS; j++){
			for(k=0; k<BLOCKS_PER_SUPERBLOCK; k++){
				cores[i].superblocks[j].blocks[k].valid_array = (char*)calloc(PAGES_PER_BLOCK, sizeof(char));
				if(cores[i].superblocks[j].blocks[k].valid_array == NULL){
					printf("Error[%s] Calloc valid_array fail\n", __FUNCTION__);
					goto fail;
				}
				else{
					memset(cores[i].superblocks[j].blocks[k].valid_array, 0, PAGES_PER_BLOCK);
					cores[i].superblocks[j].blocks[k].n_valid_pages = 0;
					cores[i].superblocks[j].blocks[k].cur_page = 0;
				}
			}
		}
	}

	return;

fail:
	printf("Error[%s] Init core structure fail\n", __FUNCTION__);
	return;
}

int get_new_page(int core, long long lpn, long long* ppn)
{
	long long new_page;
	int count = 0;

	int core_index;
	int cur_superblock;
	int cur_block;

	if(lpn == -1){
		printf("Error[%s] lpn is -1 \n", __FUNCTION__);
		return FAIL;
	}

	if(core == N_CORES){
	        if(N_CORES == 1){
        	        core_index = 0;
        	}
	        else if(N_CORES == 2 && (lpn % 2 == 0)){
        	        core_index = 0;
	        }
        	else{
	                core_index = 1;
        	}
	}
	else{
		core_index = core;
	}

retry:
	/* Get index */
	cur_superblock = cores[core_index].cur_superblock;
	cur_block = cores[core_index].superblocks[cur_superblock].cur_block;

	/* Get new page */
	new_page = cores[core_index].superblocks[cur_superblock].blocks[cur_block].cur_page;

	/* Check the page */
	if(new_page == PAGES_PER_BLOCK){

		cores[core_index].superblocks[cur_superblock].state = VICTIM;
		while(1){
			cores[core_index].cur_superblock++;
			if(cores[core_index].cur_superblock == N_SUPERBLOCKS){
				cores[core_index].cur_superblock = 0;
			}
			cur_superblock = cores[core_index].cur_superblock;

			if(cores[core_index].superblocks[cur_superblock].state == EMPTY){
				break;
			}
			else{
				count++;
				if(count > N_SUPERBLOCKS){
					printf("Error[%s] Get new superblock fail, empty_superblock: %d\n", __FUNCTION__, cores[core_index].n_empty_superblock);
					return FAIL;
				}		
			}
		}
		goto retry;
	}

	/* Return Physical page number */
	*ppn = core_index*PAGES_PER_CORE + cur_block*PAGES_PER_FLASH + cur_superblock*PAGES_PER_BLOCK + new_page;

	/* Update superblock state */	
	if(cores[core_index].superblocks[cur_superblock].state == EMPTY){
		cores[core_index].superblocks[cur_superblock].state = DIRTY;
		cores[core_index].n_empty_superblock--;
	}

//TEMP
//	printf("%d\t%d\t%lld\t%lld\n", cur_superblock, cur_block, new_page, *ppn);

	/* Update current page offset */
	cores[core_index].superblocks[cur_superblock].blocks[cur_block].cur_page++;

	/* Update current block offset */
	cores[core_index].superblocks[cur_superblock].cur_block += 1;

	cur_block = cores[core_index].superblocks[cur_superblock].cur_block;
	if(cur_block == BLOCKS_PER_SUPERBLOCK){
        	cores[core_index].superblocks[cur_superblock].cur_block = 0;
	}
	

	if(*ppn >= N_PAGES){
		printf("Error[%s] ppn: %lld\n", __FUNCTION__, *ppn);
		return FAIL;
	}

	return SUCCESS;
}

long long get_mapping(long long lpn)
{
	return mapping_table[lpn];
}

long long get_inverse_mapping(long long ppn)
{
	return inverse_mapping_table[ppn];
}

void update_mapping_table(long long lpn, long long ppn)
{
	mapping_table[lpn] = ppn;
}

void update_inverse_mapping_table(long long ppn, long long lpn)
{
	inverse_mapping_table[ppn] = lpn;
}

void update_valid_array(long long ppn, int validity)
{
	int core_index;
	long long lpn = get_inverse_mapping(ppn);

	if(lpn == -1){
		printf("Error[%s] Invalid lpn (ppn %lld)\n", __FUNCTION__, ppn);
		return;
	}

	if(N_CORES == 1){
		core_index = 0;
	}
	else if(N_CORES == 2 && (lpn % 2 == 0)){
		core_index = 0;
	}
	else{
		core_index = 1;
	}

	unsigned int superblock_nb = calc_superblock(ppn);
	unsigned int block_nb = calc_block(ppn);
	unsigned int page_nb = calc_page(ppn);

	char valid;
	
//TEMP
//	printf("%u\t%u\t%u\t%lld [%d] --> %d\n", superblock_nb, block_nb, page_nb, ppn, validity, cores[core_index].superblocks[superblock_nb].blocks[block_nb].n_valid_pages);

	if(validity == VALID){
		cores[core_index].superblocks[superblock_nb].blocks[block_nb].valid_array[page_nb] = 'V';
		cores[core_index].superblocks[superblock_nb].blocks[block_nb].n_valid_pages += 1;
		cores[core_index].superblocks[superblock_nb].n_valid_pages += 1;
	}
	else if(validity == INVALID){
		cores[core_index].superblocks[superblock_nb].blocks[block_nb].valid_array[page_nb] = 'I';
		cores[core_index].superblocks[superblock_nb].blocks[block_nb].n_valid_pages -= 1;
		cores[core_index].superblocks[superblock_nb].n_valid_pages -= 1;
	}
	else if(validity == EMPTY){
		cores[core_index].superblocks[superblock_nb].blocks[block_nb].valid_array[page_nb] = '0';
	}

}

unsigned int calc_block(long long ppn)
{
	long long core_ppn = ppn % PAGES_PER_CORE;
        unsigned int block_nb = (core_ppn/PAGES_PER_BLOCK)/N_SUPERBLOCKS;

        if(block_nb >= BLOCKS_PER_SUPERBLOCK){
                printf("Error[%s] block_nb %u (ppn %lld)\n", __FUNCTION__, block_nb, core_ppn);
        }
        return block_nb;
}

unsigned int calc_superblock(long long ppn)
{
	long long core_ppn = ppn % PAGES_PER_CORE;
        unsigned int superblock_nb = (core_ppn/PAGES_PER_BLOCK)%N_SUPERBLOCKS;

        if(superblock_nb >= N_SUPERBLOCKS){
                printf("Error[%s] superblock_nb %u\n",__FUNCTION__, superblock_nb);
        }
        return superblock_nb;
}

unsigned int calc_page(long long ppn)
{
	long long core_ppn = ppn % PAGES_PER_CORE;
        unsigned int page_nb = core_ppn % PAGES_PER_BLOCK;

        return page_nb;
}

void print_block_state(int core_index)
{
	int i, j;
	FILE* fp;

	fp = fopen("./block_state.txt", "w");

	for(i=0; i<N_SUPERBLOCKS; i++){
		for(j=0; j<BLOCKS_PER_SUPERBLOCK; j++){
			fprintf(fp,"%d\t%d\n",core_index, cores[core_index].superblocks[i].blocks[j].n_valid_pages);
		}
	}

	fclose(fp);

//	scanf("%d",&i);
}
