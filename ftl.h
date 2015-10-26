// File: ftl.h
// Date: 2016. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#ifndef _FTL_H_
#define _FTL_H_

void _ftl_write(long long sector_nb, unsigned int length);
void ftl_write(long long sector_nb, unsigned int length);

#endif
