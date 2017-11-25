#include <sys/paging.h>
#include <unistd.h>

void paging_mark_pages(int flags);

pid_t
fork(void)
{
#if 0
    // create a task_struct, copy parent's members
    // mark page tables of user space(less than kernbase) with special
    // permissions - user, read, cow.
    uint64_t pml4_vaddr;
    uint64_t cr3;
    uint64_t p_addr;
    uint64_t v_addr;
    uint64_t table_pa;
    pml4_vaddr = (~0 << 12);
    cr3 = pa = (uint64_t)paging_pagelist_get_frame();
    paging_page_copy(pml4_vaddr, PAGING_PAGE_COPY_TEMP_VA, p_addr, TRUE);
    for (int i = 0; i < PAGING_TABLE_ENTRIES - 1; i++) {
        v_addr = 0xffff;
        if (pml4_vaddr[i] & PAGING_PAGE_PRESENT) {
            table_pa = paging_pagelist_get_frame();
            pml4_vaddr[i] = talbe_pa | PAGING_PAGETABLE_PERMISSIONS;
            paging_page_copy(pml4_vaddr, PAGING_PAGE_COPY_TEMP_VA, p_addr,
                             FALSE);
            pdpe_vaddr = (pml4_vaddr << 9) | (i << 12);
            for (int j = 0; j < PAGING_TABLE_ENTRIES; j++) {
                if (pdpe[j])
            }
        }
    }
#endif
    return 0;
}
