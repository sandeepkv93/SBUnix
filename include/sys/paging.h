#ifndef _PAGING_H
#include <sys/defs.h>
#define _PAGING_H
#define PAGING_VA_MASK 0xfffffffffffff000
#define PAGING_PAGE_COPY_TEMP_VA 0xffffffffaffff000
#define PAGING_CLEAR_PAGE_VA (PAGING_PAGE_COPY_TEMP_VA + PAGING_PAGE_SIZE)
#define PAGING_KERNMEM 0xffffffff80000000
#define PAGING_VIDEO                                                           \
    (PAGING_KERNMEM + 0x1388000) // 2nd part is 5000 * PAGESIZE TODO
#define PAGING_PAGE_SIZE 4096
#define PAGING_TABLE_ENTRIES 512

#define PAGING_PAGETABLE_KERNEL_PERMISSIONS 0x3
#define PAGING_PAGETABLE_USER_PERMISSIONS 0x7
#define PAGING_PAGE_NOT_PRESENT 0x0

#define PAGING_PHYSICAL_PAGE_OFFSET(x) (x & 0xfff)
#define PAGING_PAGE_TABLE_OFFSET(x) ((x >> 12) & 0x1ff)
#define PAGING_PAGE_DIRECTORY_OFFSET(x) ((x >> 21) & 0x1ff)
#define PAGING_PD_POINTER_OFFSET(x) ((x >> 30) & 0x1ff)
#define PAGING_PML4_OFFSET(x) ((x >> 39) & 0x1ff)

#define PAGING_PAGE_PRESENT 1
#define PAGING_PAGE_W_ONLY 2
#define PAGING_PAGE_COW 512   // 9th bit as the COW bit
#define PAGING_PT_LEVEL4 1024 // 10th bit to mark page table(level 4)
#define PAGING_PML4_SELF_REFERENCING 0xfffffffffffff000
struct pagelist_t
{
    bool present;
    int ref_count;
    struct pagelist_t* next;
};
void paging_pagelist_add_addresses(uint64_t start, uint64_t end);
void paging_pagelist_create();
void* paging_pagelist_get_frame();
void paging_create_pagetables();
uint64_t* paging_get_or_create_entry(uint64_t* table, uint32_t offset,
                                     bool is_user);
bool paging_add_pagetable_mapping(uint64_t v_addr, uint64_t p_addr,
                                  bool is_user);
uint64_t* paging_get_pt_vaddr(uint64_t v_addr);
uint64_t* paging_get_create_pt_vaddr(uint64_t v_addr);
void paging_page_copy(char* source_page_va, uint64_t dest_page_pa);
void paging_flush_tlb();
uint64_t get_ref_count(uint64_t v_addr);
void paging_inc_ref_count(uint64_t v_addr);
void paging_pagelist_free_frame(uint64_t v_addr);
void paging_free_pagetables(uint64_t* page_va_addr, int level);
int paging_num_free_pages();
#endif
