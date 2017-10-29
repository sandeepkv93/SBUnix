#ifndef _VMA_H
#include <sys/defs.h>
#define _VMA_H
struct pagelist_t
{
    bool present;
    struct pagelist_t* next;
};
void vma_pagelist_add_addresses(uint64_t start, uint64_t end);
void vma_pagelist_create();
#endif
