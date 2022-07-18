#ifndef _PAGE_ALLOC_H_
#define _PAGE_ALLOC_H_

struct page
{
	unsigned long flag;
	void *free_list;
};

#endif
