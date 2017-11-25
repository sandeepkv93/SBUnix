#ifndef _PAGING_H
#include <sys/defs.h>
#define _PAGING_H

#define PAGING_PAGE_COPY_TEMP_VA 0xffffffffffff1000
#define PAGING_COW_TEMP_VA 0xffffffffffff3000
#define PAGING_KERNMEM 0xffffffff80000000
#define PAGING_VIDEO (PAGING_KERNMEM + 0x800000) // 2nd part is 2048 * PAGESIZE
#define PAGING_PAGE_SIZE 4096
#define PAGING_TABLE_ENTRIES 512

#define PAGING_PAGETABLE_PERMISSIONS 0x7
#define PAGING_PAGE_NOT_PRESENT 0x0

#define PAGING_PHYSICAL_PAGE_OFFSET(x) (x & 0xfff)
#define PAGING_PAGE_TABLE_OFFSET(x) ((x >> 12) & 0x1ff)
#define PAGING_PAGE_DIRECTORY_OFFSET(x) ((x >> 21) & 0x1ff)
#define PAGING_PD_POINTER_OFFSET(x) ((x >> 30) & 0x1ff)
#define PAGING_PML4_OFFSET(x) ((x >> 39) & 0x1ff)

#define PAGING_PAGE_PRESENT 1
#define PAGING_PAGE_W_ONLY 2
#define PAGING_PAGE_COW 512 // 10th bit as the COW bit
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
uint64_t* paging_get_pt_vaddr(uint64_t v_addr);
void paging_page_copy(uint64_t* source_page_va, uint64_t* dest_page_va,
                      uint64_t dest_page_pa, bool self_referencing);
#endif
