#include <sys/kprintf.h>
#include <sys/paging.h>
uint64_t cur_kern_heap = PAGING_VIDEO + 0x10000;

union header
{
    struct
    {
        union header* next;
        unsigned int size;
    } meta;
    long double x;
};

typedef union header Header;

Header start;
Header* freep = NULL;

void
update_pagetable(uint64_t v_addr)
{
    uint64_t p_addr = (uint64_t)paging_pagelist_get_frame();
    paging_add_pagetable_mapping(v_addr, p_addr);
}

void*
alloc_get_page()
{
    uint64_t* v_addr = (uint64_t*)cur_kern_heap;
    // kprintf("new page %p\n", v_addr);
    update_pagetable(cur_kern_heap);
    cur_kern_heap += PAGING_PAGE_SIZE;
    return (void*)v_addr;
}

void
alloc_free_page(void* v_addr)
{
    // TODO Set page table entry to not present
}

void*
pls_giv_mem(int num_bytes)
{
    static void* cur_page_va = NULL;
    static int cur_page_offset = 0;
    int num_of_pages;
    void* va_addr;

    if (cur_page_va == NULL) {
        cur_page_va = alloc_get_page();
    }

    va_addr = cur_page_va + cur_page_offset;
    if (num_bytes > (PAGING_PAGE_SIZE - cur_page_offset)) {
        num_bytes = num_bytes - (PAGING_PAGE_SIZE - cur_page_offset);
        num_of_pages = ((num_bytes - 1) / PAGING_PAGE_SIZE) + 1;

        while (num_of_pages--) {
            cur_page_va = alloc_get_page();
        }
        cur_page_offset += num_bytes % PAGING_PAGE_SIZE;
    } else {
        cur_page_offset += num_bytes;
    }
    return va_addr;
}

void
kfree(void* new)
{
    Header* temp = freep->meta.next;
    Header* new_block;
    if (new == NULL) {
        return;
    }
    new_block = (Header*)new - 1;
    /*coalase free VAs. Walk over the free list, free when the size of a header
     * is greater than 4096. If it is greater, create a header in the next page,
     * and update size. Coalase this too. This is to avoid fragmentation. Loop
     * over the list until no more pages can be freed.
    */

    // add free'd nodes in the increasing order of addresses.

    /* coalase while adding a node to the free list. If the size is greater than
     * or equal to the page size, call a walk_free_list function that walks over
     * it, and frees the page, splits the node if rewuires.
    */
    /*
    Spend time on testing. This won't break now, this will break during
    evaluation.
    */
    /*
        Header * new_temp = freep;
        bool inserted = FALSE;
        int inserted_size;
        while(new_temp < new_block)
              new_temp = new_temp->meta.next;
        if(new_temp + new_temp->meta.size + 1 == new_block)
           {
               new_temp->meta.size = new_temp->meta.size + new_block->meta.size
       + 1;           inserted = TRUE;
               inserted_size = new_temp->meta.size; //+1 for header?
           }
        if(new_block + new_block->meta.size + 1 == new_temp->meta.next )
          {
               new_block->meta.size = new_block->meta.size + new_temp->meta.size
       +1;           new_block->meta.next = new_temp->meta.next->meta.next;
               new_temp->meta.next = new_block;
               inserted = TRUE;
               inserted_size = new_block->meta.size;
          }
        if(!inserted)
          {
               new_block->meta.next = new_temp->meta.next;
               new_temp->meta.next = new_block;
               inserted_size = new_block->meta.size;
          }
          if(inserted_size>=PAGING_PAGE_SIZE)
               find_and_free_page(node,inerted_size);
    */
    while (temp->meta.next != freep)
        temp = temp->meta.next;
    temp->meta.next = new_block;
    new_block->meta.next = freep;
}

void
print_kfree_list()
{
    Header* temp = freep->meta.next;
    while (temp != freep) {
        temp = temp->meta.next;
    }
}

void
print_kmalloc_list()
{
    Header* temp = start.meta.next;
    while (temp != &start) {
        temp = temp->meta.next;
    }
}
Header*
get_mem(unsigned num_units)
{
    Header* new;
    new = (Header*)pls_giv_mem(num_units * sizeof(Header));
    new->meta.size = num_units;
    kfree((void*)(new + 1));
    return freep;
}

void*
kmalloc(size_t num_bytes)
{
    unsigned num_units;
    Header *cur, *prev;
    if (freep == NULL) {
        start.meta.next = freep = prev = &start;
        start.meta.size = 0;
    }
    num_units = ((num_bytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
    prev = freep;
    cur = prev->meta.next;
    while (1) {
        if (cur->meta.size >= num_units) {
            prev->meta.next = cur->meta.next;
            return (void*)(cur + 1);
        }
        if (cur == freep) {
            cur = get_mem(num_units);
        }
        prev = cur;
        cur = cur->meta.next;
    }
}

/*
void brk(void * brk_inc)
{
    struct vma_struct * vma_temp;
    uint64_t p_addr;
    vma_temp = task_get_this_task_struct()->vma_list;
    while(vma_temp->type != VMA_HEAP)
       vma_temp=vma_temp->next;
if(brk_inc == NULL or (uint64_t)brk_inc <= 0)
 {
   return vma_temp->end;
 }

//we won't reduce the program data section

 if(((vma_temp->end % PAGING_PAGE_SIZE) + brk_inc) > PAGING_PAGE_SIZE)
     {
         //alloc page
         vma_temp->end+=brk_inc;
         p_addr = (uint64_t)paging_pagelist_get_frame();
         paging_add_pagetable_mapping(vma_temp->end & PAGING_VA_MASK ,
                                             p_addr);
     }

}
*/
