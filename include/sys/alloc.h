#ifndef _ALLOC_H
#define _ALLOC_H
#include <unistd.h>
void* kmalloc(size_t size);
void* kfree(void* pt);
void* get_free_pages();

#endif
