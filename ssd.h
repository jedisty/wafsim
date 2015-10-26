// File: ssd.h
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#ifndef _SSD_H_
#define _SSD_H_

void ssd_page_write(long long ppn, int io_type);
void ssd_page_partial_write(long long old_ppn, long long new_ppn);

#endif
