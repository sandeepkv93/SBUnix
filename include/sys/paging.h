#ifndef _PAGING_H
#include <sys/defs.h>
#define _PAGING_H

#define PAGING_KERNMEM 0xffffffff80000000
#define PAGING_VIDEO (PAGING_KERNMEM + 0x800000) // 2nd part is 2048 * PAGESIZE
#define PAGING_PAGE_SIZE 4096

#define PAGING_PHYSICAL_PAGE_OFFSET(x) (x & 0xfff)
#define PAGING_PAGE_TABLE_OFFSET(x) ((x >> 12) & 0x1ff)
#define PAGING_PAGE_DIRECTORY_OFFSET(x) ((x >> 21) & 0x1ff)
#define PAGING_PD_POINTER_OFFSET(x) ((x >> 30) & 0x1ff)
#define PAGING_PML4_OFFSET(x) ((x >> 39) & 0x1ff)

struct pagelist_t
{
    bool present;
    struct pagelist_t* next;
};
void paging_pagelist_add_addresses(uint64_t start, uint64_t end);
void paging_pagelist_create();
void* paging_pagelist_get_frame();
void paging_create_pagetables();
uint64_t* paging_get_table_entry(uint64_t* table, uint32_t offset);
bool paging_add_pagetable_mapping(uint64_t v_addr, uint64_t p_addr);
void paging_pagelist_free_frame();
#endif
