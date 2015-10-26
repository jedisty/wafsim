// File: wafsim.h
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ftl.h"
#include "ssd.h"
#include "mapping.h"
#include "gc.h"

#ifndef _WAFSIM_H_
#define _WAFSIM_H_

#define SUCCESS		0
#define FAIL		1

#define WRITE		10
#define GC_WRITE	11

#define EMPTY		100
#define VALID		101
#define INVALID		102
#define DIRTY		103
#define VICTIM		104

/* SSD Configuration */
//#define PAGE_SIZE       8192
//#define SECTOR_SIZE     512
//#define N_CHANNEL       4
//#define N_WAY           4
//#define N_PLAIN         4
//#define PAGES_PER_BLOCK 512
//#define N_SUPERBLOCKS   960
//#define N_CORES         1

#define PAGE_SIZE	8192
#define SECTOR_SIZE	512
#define N_CHANNEL	1
#define N_WAY		1
#define N_PLAIN		1
#define PAGES_PER_BLOCK	512
//#define N_SUPERBLOCKS	61440	//240GB
#define N_SUPERBLOCKS	63180	//246.8GB
#define N_CORES		1

/* Initialized at start */
long long BLOCK_SIZE;
long long SUPERBLOCK_SIZE;
long long N_SECTORS;
long long SECTORS_PER_PAGE;
long long N_PAGES;
long long BLOCKS_PER_SUPERBLOCK;
long long PAGES_PER_SUPERBLOCK;
long long BLOCKS_PER_FLASH;
long long PAGES_PER_CORE;
long long PAGES_PER_FLASH;

/* global variable for WAF */
extern long long n_written_pages;
extern long long n_gc_written_pages;
extern long long n_total_sectors;

void init_wafsim(void);
void show_result(void);

#endif
