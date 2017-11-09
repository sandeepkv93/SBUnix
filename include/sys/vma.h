#ifndef _VMA_H
#include <sys/defs.h>
#define _VMA_H

#define VMA_KERNMEM 0xffffffff80000000
#define VMA_VIDEO (VMA_KERNMEM + 0x800000) // 2nd part is 2048 * PAGESIZE
#define PAGELIST_ENTRIES (1024 * 1024)
#define PAGE_SIZE 4096
#define TABLE_ENTRIES 512
#define TEST_VMA (VMA_KERNMEM + 0x801000)

#define VMA_PHYSICAL_PAGE_OFFSET(x) (x & 0xfff)
#define VMA_PAGE_TABLE_OFFSET(x) ((x >> 12) & 0x1ff)
#define VMA_PAGE_DIRECTORY_OFFSET(x) ((x >> 21) & 0x1ff)
#define VMA_PD_POINTER_OFFSET(x) ((x >> 30) & 0x1ff)
#define VMA_PML4_OFFSET(x) ((x >> 39) & 0x1ff)

struct pagelist_t
{
    bool present;
    struct pagelist_t* next;
};
void vma_pagelist_add_addresses(uint64_t start, uint64_t end);
void vma_pagelist_create();
void vma_create_pagetables();
void vma_add_pagetable_mapping_va(uint64_t v_addr);
void* vma_pagelist_getpage();
#endif
