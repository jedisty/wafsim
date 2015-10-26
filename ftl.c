// File: ftl.c
// Date: 2015. 07. 27. 
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr) 
// Copyright(c)2015
// Hanyang University, Seoul, Korea 
// Embedded Software Systems Laboratory. All right reserved

#include "wafsim.h"

void ftl_write(long long sector_nb, unsigned int length)
{
        _ftl_write(sector_nb, length);
}

void _ftl_write(long long sector_nb, unsigned int length)
{
	long long lba = sector_nb;
	unsigned int remain = length;
	unsigned int left_skip = sector_nb % SECTORS_PER_PAGE;
	unsigned int right_skip;
	unsigned int write_sects;

	int ret = FAIL;
	long long lpn, old_ppn, new_ppn;

	if(sector_nb + length > N_SECTORS){
                printf("Error[%s] Exceed Sector number\n", __FUNCTION__);
                return;
        }

	gc_check();
	
	while(remain > 0){
		if(remain > SECTORS_PER_PAGE - left_skip){
			right_skip = 0;
		}
		else{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		write_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		lpn = lba / SECTORS_PER_PAGE;
		old_ppn = get_mapping(lpn);

		/* Get new empty page to write */
		ret = get_new_page(N_CORES, lpn, &new_ppn);
		if(ret == FAIL){
			printf("Error[%s] get new page return fail\n", __FUNCTION__);
			return;
		}

		/* Write the page to NAND */
		if((left_skip || right_skip) && (old_ppn != -1)){
			ssd_page_partial_write(old_ppn, new_ppn);
		}
		else{
			ssd_page_write(new_ppn, WRITE);
		}

		/* update mapping & inverse mapping table */
		if(old_ppn != -1){
			update_valid_array(old_ppn, INVALID);
			update_inverse_mapping_table(old_ppn, -1);
		}
		update_mapping_table(lpn, new_ppn);

		update_inverse_mapping_table(new_ppn, lpn);
		update_valid_array(new_ppn, VALID);

		lba += write_sects;
		remain -= write_sects;
		left_skip = 0;
	}

	n_total_sectors += length;

	return;
}
