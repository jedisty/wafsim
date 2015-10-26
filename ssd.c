// File: ssd.c
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#include "wafsim.h"

void ssd_page_write(long long ppn, int io_type)
{
	n_written_pages++;

	if(io_type == GC_WRITE){
		n_gc_written_pages++;
	}
}

void ssd_page_partial_write(long long old_ppn, long long new_ppn)
{
	n_written_pages++;
}
