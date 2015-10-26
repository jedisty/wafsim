// File: gc.h
// Date: 2015. 07. 29.
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#ifndef _GC_H_
#define _GC_H_

void gc_check(void);
void garbage_collection(int core_index);
int select_victim_superblock(int core_index, long long* victim_superblock);
void copy_valid_pages(int core_index, long long victim_superblock);

#endif
