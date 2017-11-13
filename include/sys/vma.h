#ifndef _VMA_H
#include <sys/defs.h>
#define _VMA_H

#define VMA_KERNMEM 0xffffffff80000000
#define VMA_VIDEO (VMA_KERNMEM + 0x800000) // 2nd part is 2048 * PAGESIZE
#define VMA_PAGE_SIZE 4096

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
void* vma_pagelist_getpage();
void vma_create_pagetables();
void* vma_pagelist_getpage();
uint64_t* vma_get_table_entry(uint64_t* table, uint32_t offset);
bool vma_add_pagetable_mapping(uint64_t v_addr, uint64_t p_addr);
#endif
