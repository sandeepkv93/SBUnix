#include <sys/kprintf.h>
#include <sys/vma.h>

#define VMA_KERNMEM 0xffffffff80000000
#define VMA_VIDEO (VMA_KERNMEM + 0x800000) // 2nd part is 2048 * PAGESIZE
#define PAGELIST_ENTRIES (1024 * 1024)
#define PAGE_SIZE 4096
#define TABLE_ENTRIES 512

#define VMA_PHYSICAL_PAGE_OFFSET(x) (x & 0xfff)
#define VMA_PAGE_TABLE_OFFSET(x) ((x >> 12) & 0x1ff)
#define VMA_PAGE_DIRECTORY_OFFSET(x) ((x >> 21) & 0x1ff)
#define VMA_PD_POINTER_OFFSET(x) ((x >> 30) & 0x1ff)
#define VMA_PML4_OFFSET(x) ((x >> 39) & 0x1ff)

struct pagelist_t pages[PAGELIST_ENTRIES];
struct pagelist_t* freepage_head;
extern void paging_enable(void*);

void
vma_pagelist_add_addresses(uint64_t start, uint64_t end)
{
    // Call with start and end physical addresses that you want to
    // be bookkept in the FREELIST
    for (int i = start / PAGE_SIZE; i < end / PAGE_SIZE; i++) {
        pages[i].present = TRUE;
    }
}

void
vma_pagelist_create(uint64_t physfree)
{
    // Once you add all addresses to be bookkept in the FREELIST by
    // calling vma_pagelist_add_addresses, call vma_pagelist_create
    // The parameter signifies the page boundary from where free
    // pages are assigned

    int i = physfree / PAGE_SIZE;
    bool isLastPage = FALSE;
    while (!isLastPage) {
        isLastPage = TRUE;
        for (int j = i + 1; j < PAGELIST_ENTRIES; j++) {
            if (pages[j].present) {
                pages[i].next = &pages[j];
                isLastPage = FALSE;
                i = j;
                break;
            }
        }
    }
    freepage_head = &pages[(physfree / PAGE_SIZE)];
}

void*
vma_pagelist_getpage()
{
    // Returns a freepage from the FREELIST
    uint64_t pageAddress = (freepage_head - pages) * PAGE_SIZE;
    if (freepage_head == NULL) {
        return NULL;
        kprintf("Out of memory");
        while (1)
            ;
    }
    freepage_head = freepage_head->next;
    return (void*)pageAddress;
}

uint64_t*
vma_add_table_mapping(uint64_t* table, uint32_t offset)
{
    // Adds entry into offset of the table if not present, returns entry
    char* temp_byte;
    if (!(table[offset] & 0x1)) {
        table[offset] = (uint64_t)vma_pagelist_getpage();
        temp_byte = (char*)table[offset];
        for (int i = 0; i < PAGE_SIZE; i++) {
            temp_byte[i] = 0;
        }
        table[offset] |= 0x3;
    }
    return (uint64_t*)(table[offset] & 0xfffffffffffff000);
}

void
vma_create_pagetables()
{
    // Creates the 4 level pagetables needed and switches CR3
    uint64_t* pml4_table = vma_pagelist_getpage();
    uint64_t* pdp_table;
    uint64_t* pd_table;
    uint64_t* pt_table;
    uint64_t v_addr;

    for (int i = 0; i < TABLE_ENTRIES; i++) {
        pml4_table[i] = 0;
    }

    // Self referencing trick
    pml4_table[TABLE_ENTRIES - 1] = ((uint64_t)pml4_table) | 0x3;

    // TODO Remove this hard coding of 2048. Map only physbase to physfree
    for (int i = 0; i < 4735; i++) {
        v_addr = VMA_KERNMEM + i * (PAGE_SIZE);
        pdp_table = vma_add_table_mapping(pml4_table, VMA_PML4_OFFSET(v_addr));
        pd_table =
          vma_add_table_mapping(pdp_table, VMA_PD_POINTER_OFFSET(v_addr));
        pt_table =
          vma_add_table_mapping(pd_table, VMA_PAGE_DIRECTORY_OFFSET(v_addr));

        pt_table[VMA_PAGE_TABLE_OFFSET(v_addr)] = (0x0 + i * (PAGE_SIZE)) | 0x3;
    }

    // TODO add new function to create the mapping making the following code
    // generic
    v_addr = VMA_VIDEO;
    pdp_table = vma_add_table_mapping(pml4_table, VMA_PML4_OFFSET(v_addr));
    pd_table = vma_add_table_mapping(pdp_table, VMA_PD_POINTER_OFFSET(v_addr));
    pt_table =
      vma_add_table_mapping(pd_table, VMA_PAGE_DIRECTORY_OFFSET(v_addr));
    pt_table[VMA_PAGE_TABLE_OFFSET(v_addr)] = (0xb8000) | 0x3;

    paging_enable(pml4_table);
}