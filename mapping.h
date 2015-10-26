// File: mapping.h
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#ifndef _MAPPING_H_
#define _MAPPING_H_

extern long long* mapping_table;
extern long long* inverse_mapping_table;
extern struct core* cores;

typedef struct block
{
	char* valid_array;
	int n_valid_pages;
	int cur_page;
}block;

typedef struct superblock{
	block* blocks;
	int n_valid_pages;
	int state;
	int cur_block;
}superblock;

typedef struct core{
	superblock* superblocks;
	int n_empty_superblock;
	int cur_superblock;
}core;

void init_cores(void);
void init_mapping_table(void);
void init_inverse_mapping_table(void);

int get_new_page(int core, long long lpn, long long* ppn);
long long get_mapping(long long lpn);

void update_mapping_table(long long lpn, long long ppn);
void update_inverse_mapping_table(long long ppn, long long lpn);

unsigned int calc_block(long long ppn);
unsigned int calc_superblock(long long ppn);
unsigned int calc_page(long long ppn);

void print_block_state(int core_index);

#endif
