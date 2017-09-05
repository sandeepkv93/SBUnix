#include <stdlib.h>
union header
{
	struct
	{
		union header  *next;
		unsigned int size;
	}meta;
	long double x; //for alignment to support the most restrictive type
};

typedef union header Header;

Header start;
Header *freep = NULL;

void *brk_new(void * addr);
void *pls_giv_mem(int num_bytes);
void free_(void * new);
Header * get_mem(unsigned num_units);
void* malloc_(unsigned num_bytes);
void print_free_list();
void print_malloc_list();

