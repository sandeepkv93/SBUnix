#ifndef _ALLOC_H
#define _ALLOC_H
#include <sys/defs.h>
void* kmalloc(size_t size);
void* kfree(void* pt);
void* get_free_page();
void update_pagetable(uint64_t v_addr);

#endif
