#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <test.h>

#define PAGING_PAGELIST_ENTRIES (1024 * 1024)
#define PAGING_TABLE_ENTRIES 512
#define TEST_PAGING (PAGING_KERNMEM + 0x801000)
// TODO Allocate pages array dynamically
struct pagelist_t pages[PAGING_PAGELIST_ENTRIES];
struct pagelist_t* freepage_head;
extern void paging_enable(void*);
extern int paging_flush_tlb_asm();

int intial_page_mapping_done = 0;

void
update_pt_entry(uint64_t v_addr, uint64_t addr_flags)
{
    uint64_t* pt_table = paging_get_pt_vaddr(v_addr);
    uint64_t pt_offset = PAGING_PAGE_TABLE_OFFSET(v_addr);
    pt_table[pt_offset] = addr_flags;
}

void
paging_pagelist_add_addresses(uint64_t start, uint64_t end)
{
    // Call with start and end physical addresses that you want to
    // be bookkept in the FREELIST
    for (int i = start / PAGING_PAGE_SIZE; i < end / PAGING_PAGE_SIZE; i++) {
        pages[i].present = TRUE;
        pages[i].ref_count = 0;
    }
}

uint64_t
get_ref_count(uint64_t v_addr)
{
    uint64_t frame_addr, index;
    uint64_t* pt_table = paging_get_pt_vaddr(v_addr);
    uint64_t pt_offset = PAGING_PAGE_TABLE_OFFSET(v_addr);
    frame_addr = pt_table[pt_offset]; // flags get removed after division
    index = frame_addr / PAGING_PAGE_SIZE;
    return pages[index].ref_count;
}

void
paging_inc_ref_count(uint64_t v_addr)
{
    uint64_t frame_addr, index;
    uint64_t* pt_table = paging_get_pt_vaddr(v_addr);
    uint64_t pt_offset = PAGING_PAGE_TABLE_OFFSET(v_addr);
    frame_addr = pt_table[pt_offset]; // flags get removed after division
    index = frame_addr / PAGING_PAGE_SIZE;
    pages[index].ref_count++;
}

int
paging_pagelist_create(uint64_t physfree)
{
    // Once you add all addresses to be bookkept in the FREELIST by
    // calling paging_pagelist_add_addresses, call paging_pagelist_create
    // The parameter signifies the page boundary from where free
    // pages are assigned

    int i = physfree / PAGING_PAGE_SIZE;
    bool isLastPage = FALSE;
    while (!isLastPage) {
        isLastPage = TRUE;
        for (int j = i + 1; j < PAGING_PAGELIST_ENTRIES; j++) {
            if (pages[j].present) {
                pages[i].next = &pages[j];
                isLastPage = FALSE;
                i = j;
                break;
            }
        }
    }
    freepage_head = &pages[(physfree / PAGING_PAGE_SIZE)];
    return i;
}

void
clear_pages(uint64_t pageAddress)
{
    uint64_t* pagetable;
    uint64_t pt_offset;
    char* temp_va = (char*)PAGING_CLEAR_PAGE_VA;
    paging_flush_tlb();
    pagetable = paging_get_pt_vaddr((uint64_t)temp_va);
    pt_offset = PAGING_PAGE_TABLE_OFFSET((uint64_t)temp_va);
    pagetable[pt_offset] =
      (uint64_t)pageAddress | PAGING_PAGETABLE_KERNEL_PERMISSIONS;
    for (int i = 0; i < PAGING_PAGE_SIZE; i++) {
        temp_va[i] = 0;
    }
    // TODO:setting pt entry to not present required?
    paging_flush_tlb();
}

void*
paging_pagelist_get_frame()
{
    // Returns a freepage from the FREELIST
    uint64_t pageAddress = (freepage_head - pages) * PAGING_PAGE_SIZE;
    if (freepage_head == NULL) {
        kprintf("Out of memory");
        while (1)
            ;
    }
    if (intial_page_mapping_done) {
        clear_pages(pageAddress);
    }
    freepage_head->ref_count = 1;
    freepage_head = freepage_head->next;
    return (void*)pageAddress;
}

void
add_free_frame(uint64_t index)
{
    // TODO add to end of list
    pages[index].next = freepage_head;
    freepage_head = &pages[index];
}

void
paging_pagelist_free_frame(uint64_t v_addr)
{
    uint64_t frame_addr, index;
    uint64_t* pt_table = paging_get_pt_vaddr(v_addr);
    uint64_t pt_offset = PAGING_PAGE_TABLE_OFFSET(v_addr);
    frame_addr = pt_table[pt_offset] &
                 0xfffffffffffff000; // flags get removed after division
    index = frame_addr / PAGING_PAGE_SIZE;
    pages[index].ref_count--;
    if (pages[index].ref_count == 0) {
        kprintf("Will free VA %x, index %d\n", v_addr, index);
        /*if (v_addr < 0x700000)*/
        /*while (1)*/
        /*;*/
        add_free_frame(index);
    }
    paging_flush_tlb();
}

uint64_t*
paging_get_table_entry(uint64_t* table, uint32_t offset)
{
    if (!(table[offset] & 0x1)) {
        // TODO remove this
        kprintf("No PT entry");
        while (1)
            ;
        return NULL;
    }
    return (uint64_t*)(table[offset] & 0xfffffffffffff000);
}

uint64_t*
paging_get_or_create_entry(uint64_t* table, uint32_t offset, bool is_user)
{
    // Adds entry into offset of the table if not present, returns entry
    if (!(table[offset] & 0x1)) {
        table[offset] = (uint64_t)paging_pagelist_get_frame();

        /*
        // TODO Decide if it is needed to set page to 0
        temp_byte = (char*)table[offset];
        for (int i = 0; i < PAGING_PAGE_SIZE; i++) {
            temp_byte[i] = 0;
        }
        */
        if (is_user) {
            table[offset] |= PAGING_PAGETABLE_USER_PERMISSIONS;
        } else {
            table[offset] |= PAGING_PAGETABLE_KERNEL_PERMISSIONS;
        }
    }
    return (uint64_t*)(table[offset] & 0xfffffffffffff000);
}

uint64_t*
paging_get_create_pt_vaddr(uint64_t v_addr)
{
    // calling this creates entries in intermediate page tables(or new
    // intermdediate page allocatins if table is not present) since we call
    // paging_get_or_create_entry.
    uint64_t pml4_vaddr, pdp_vaddr, pd_vaddr, pt_vaddr;
    bool is_user = FALSE;
    if (v_addr < PAGING_KERNMEM) {
        is_user = TRUE;
    }
    pml4_vaddr = PAGING_PML4_SELF_REFERENCING;
    paging_get_or_create_entry((uint64_t*)pml4_vaddr,
                               PAGING_PML4_OFFSET(v_addr), is_user);

    pdp_vaddr = ((pml4_vaddr << 9) | (PAGING_PML4_OFFSET(v_addr) << 12));
    paging_get_or_create_entry((uint64_t*)pdp_vaddr,
                               PAGING_PD_POINTER_OFFSET(v_addr), is_user);

    pd_vaddr = ((pdp_vaddr << 9) | (PAGING_PD_POINTER_OFFSET(v_addr) << 12));
    paging_get_or_create_entry((uint64_t*)pd_vaddr,
                               PAGING_PAGE_DIRECTORY_OFFSET(v_addr), is_user);

    pt_vaddr = ((pd_vaddr << 9) | (PAGING_PAGE_DIRECTORY_OFFSET(v_addr) << 12));
    return (uint64_t*)pt_vaddr;
}

uint64_t*
paging_get_pt_vaddr(uint64_t v_addr)
{
    uint64_t pml4_vaddr, pdp_vaddr, pd_vaddr, pt_vaddr;
    pml4_vaddr = PAGING_PML4_SELF_REFERENCING;
    paging_get_table_entry((uint64_t*)pml4_vaddr, PAGING_PML4_OFFSET(v_addr));

    pdp_vaddr = ((pml4_vaddr << 9) | (PAGING_PML4_OFFSET(v_addr) << 12));
    paging_get_table_entry((uint64_t*)pdp_vaddr,
                           PAGING_PD_POINTER_OFFSET(v_addr));

    pd_vaddr = ((pdp_vaddr << 9) | (PAGING_PD_POINTER_OFFSET(v_addr) << 12));
    paging_get_table_entry((uint64_t*)pd_vaddr,
                           PAGING_PAGE_DIRECTORY_OFFSET(v_addr));

    pt_vaddr = ((pd_vaddr << 9) | (PAGING_PAGE_DIRECTORY_OFFSET(v_addr) << 12));
    return (uint64_t*)pt_vaddr;
}

bool
paging_add_pagetable_mapping(uint64_t v_addr, uint64_t p_addr, bool is_user)
{
    // The pagetables' addresses can be calculated because of self referencing
    // trick
    uint64_t* pagetable;
    uint32_t pt_offset;

    // Flush not needed
    pt_offset = PAGING_PAGE_TABLE_OFFSET(v_addr);
    pagetable = paging_get_create_pt_vaddr(v_addr);
    paging_flush_tlb();

    if ((pagetable[pt_offset] & PAGING_PAGE_PRESENT)) {
        // TODO decrement ref count?
        pagetable[pt_offset] = p_addr;
        pagetable[pt_offset] |= PAGING_PT_LEVEL4;
        if (is_user) {
            pagetable[pt_offset] |= PAGING_PAGETABLE_USER_PERMISSIONS;
        } else {
            pagetable[pt_offset] |= PAGING_PAGETABLE_KERNEL_PERMISSIONS;
        }
        paging_flush_tlb();
        return FALSE;
    } else {
        pagetable[pt_offset] = p_addr;
        /*
        // TODO Decide if it is needed to set page to 0
        char* temp_byte = (char*)pagetable[pt_offset];
        for (int i = 0; i < PAGE_SIZE; i++) {
            temp_byte[i] = 0;
        }
        */
        pagetable[pt_offset] |= PAGING_PT_LEVEL4;
        if (is_user) {
            pagetable[pt_offset] |= PAGING_PAGETABLE_USER_PERMISSIONS;
        } else {
            pagetable[pt_offset] |= PAGING_PAGETABLE_KERNEL_PERMISSIONS;
        }
        return TRUE;
    }
}

void
paging_add_initial_pagetable_mapping(uint64_t* pml4_phys_addr, uint64_t v_addr,
                                     uint64_t phys_entry)
{
    // This function can add mapping if the physcial <-> virtual mapping is 1:1
    // Thus can be used to setup initial pagetables only
    uint64_t* pdp_table;
    uint64_t* pd_table;
    uint64_t* pt_table;

    pdp_table = paging_get_or_create_entry(pml4_phys_addr,
                                           PAGING_PML4_OFFSET(v_addr), FALSE);

    pd_table = paging_get_or_create_entry(
      pdp_table, PAGING_PD_POINTER_OFFSET(v_addr), FALSE);

    pt_table = paging_get_or_create_entry(
      pd_table, PAGING_PAGE_DIRECTORY_OFFSET(v_addr), FALSE);

    pt_table[PAGING_PAGE_TABLE_OFFSET(v_addr)] = phys_entry | PAGING_PT_LEVEL4;
}

void
paging_clear_initial_memory(uint64_t physfree)
{
    uint64_t* temp = (uint64_t*)physfree;
    /*while ((uint64_t)temp < 0x40000000) {*/
    while ((uint64_t)temp < physfree + 100 * PAGING_PAGE_SIZE) {
        *temp = 0;
        temp++;
    }
}
void
paging_create_pagetables(uint64_t physbase, uint64_t physfree)
{
    // Creates the 4 level pagetables needed and switches CR3

    uint64_t* pml4_table;
    uint64_t v_addr;
    paging_clear_initial_memory(physfree);
    pml4_table = paging_pagelist_get_frame();
    for (int i = 0; i < PAGING_TABLE_ENTRIES; i++) {
        pml4_table[i] = 0;
    }

    // Self referencing trick
    pml4_table[PAGING_TABLE_ENTRIES - 1] =
      ((uint64_t)pml4_table) | PAGING_PAGETABLE_USER_PERMISSIONS;

    // TODO Remove this hard coding of 2048. Map only physbase to physfree
    for (int i = physbase / PAGING_PAGE_SIZE; i < physfree / PAGING_PAGE_SIZE;
         i++) {
        v_addr = PAGING_KERNMEM + i * (PAGING_PAGE_SIZE);
        paging_add_initial_pagetable_mapping(
          pml4_table, v_addr,
          (i * PAGING_PAGE_SIZE) | PAGING_PAGETABLE_KERNEL_PERMISSIONS);
    }

    paging_add_initial_pagetable_mapping(pml4_table, PAGING_VIDEO,
                                         (0xb8000) |
                                           PAGING_PAGETABLE_KERNEL_PERMISSIONS);
    // temp_va_mapping for clearing pages
    paging_add_initial_pagetable_mapping(
      pml4_table, PAGING_CLEAR_PAGE_VA, ((uint64_t)paging_pagelist_get_frame() |
                                         PAGING_PAGETABLE_KERNEL_PERMISSIONS));
    paging_enable(pml4_table);
    intial_page_mapping_done = 1;
}

void
paging_page_copy(char* source_page_va, uint64_t dest_page_pa)
{
    char* dest_page_va = (char*)PAGING_PAGE_COPY_TEMP_VA;
    uint64_t* pagetable;
    uint64_t pt_offset;
    paging_add_pagetable_mapping((uint64_t)dest_page_va, dest_page_pa, FALSE);
    for (int i = 0; i < PAGING_PAGE_SIZE; i++) {
        dest_page_va[i] = source_page_va[i];
    }
    pagetable = paging_get_pt_vaddr((uint64_t)dest_page_va);
    pt_offset = PAGING_PAGE_TABLE_OFFSET((uint64_t)dest_page_va);
    pagetable[pt_offset] = PAGING_PAGE_NOT_PRESENT;
    paging_flush_tlb();
}

int
paging_num_free_pages()
{
    int count = 0;
    struct pagelist_t* temp = freepage_head;
    while (temp != NULL) {
        temp = temp->next;
        count++;
    }
    return count;
}

void
paging_flush_tlb()
{
    paging_flush_tlb_asm();
}

void
paging_free_pagetables(uint64_t* page_va_addr, int level)
{
    uint64_t next_table_addr;

    for (int i = 0; i < PAGING_TABLE_ENTRIES; i++) {

        if (!(page_va_addr[i] & PAGING_PAGE_PRESENT) ||
            (level == 1 && i >= PAGING_TABLE_ENTRIES - 2)) {
            continue;
        }

        next_table_addr = ((uint64_t)page_va_addr << 9) | (i << 12);
        // TODO: ensure that pt_level4 entry is being added everywhere
        if (page_va_addr[i] & PAGING_PT_LEVEL4) {
            paging_pagelist_free_frame(next_table_addr);
            paging_flush_tlb();

        } else {

            paging_free_pagetables((uint64_t*)next_table_addr, level + 1);
        }
    }
    paging_pagelist_free_frame((uint64_t)page_va_addr);
    paging_flush_tlb();
}
